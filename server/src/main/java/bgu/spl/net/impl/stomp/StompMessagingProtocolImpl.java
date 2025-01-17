
import java.util.concurrent.ConcurrentHashMap;
import bgu.spl.net.api.*;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.impl.stomp.*;;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate = false;
    private UserMeneger userMeneger;  // Reference to UserManager


    // Store user subscriptions
    private ConcurrentHashMap<String, Integer> subscriptions = new ConcurrentHashMap<>();

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        String[] lines = message.split("\n", -1);
        String command = lines[0];

        switch (command) {
            case "CONNECT":
                handleConnect(lines);
                break;
            case "SUBSCRIBE":
                handleSubscribe(lines);
                break;
            case "UNSUBSCRIBE":
                handleUnsubscribe(lines);
                break;
            case "SEND":
                handleSend(lines);
                break;
            case "DISCONNECT":
                handleDisconnect(lines);
                break;
            default:
                handleError("Unknown command: " + command);
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
    private void handleConnect(String[] lines) {
        String username = getHeader(lines, "login");
        String password = getHeader(lines, "passcode");
    
        if (username == null || password == null) {
            handleError("Missing login or passcode in CONNECT frame.");
            return;
        }
    
        boolean loginSuccessful = userMeneger.login(username, password, connectionId);
    
        if (loginSuccessful) {
            connections.send(connectionId, "CONNECTED\nversion:1.2\n\n");
        } else {
            handleError("Login failed: User already logged in or incorrect credentials.");
        }    }

    private void handleSubscribe(String[] lines) {
        String destination = getHeader(lines, "destination");
        String subscriptionId = getHeader(lines, "id");
        String receipt = getHeader(lines, "receipt"); // Optional receipt header
    
        if (destination != null && subscriptionId != null) {
            subscriptions.put(destination, Integer.parseInt(subscriptionId));
            connections.subscribe(connectionId, destination);
    
            if (receipt != null) {
                connections.send(connectionId, "RECEIPT\nreceipt-id:" + receipt + "\n\n");
            }
        } else {
            handleError("Invalid SUBSCRIBE frame: Missing headers.");
        }
    }

    private void handleUnsubscribe(String[] lines) {
        String subscriptionId = getHeader(lines, "id");
        String receipt = getHeader(lines, "receipt"); // Optional receipt header
    
        if (subscriptionId != null) {
            subscriptions.values().remove(Integer.parseInt(subscriptionId));
            connections.unsubscribe(connectionId, subscriptionId);
    
            if (receipt != null) {
                connections.send(connectionId, "RECEIPT\nreceipt-id:" + receipt + "\n\n");
            }
        } else {
            handleError("Invalid UNSUBSCRIBE frame: Missing 'id' header.");
        }
    }

    private void handleSend(String[] lines) {
        String destination = getHeader(lines, "destination");
        String body = getBody(lines);
    
        if (destination != null && subscriptions.containsKey(destination)) {
            connections.send(destination, body);
        } else {
            handleError("Invalid SEND frame: Client not subscribed to destination or missing 'destination' header.");
        }
    }

    private void handleDisconnect(String[] lines) {
        String receipt = getHeader(lines, "receipt");

    if (receipt != null) {
        connections.send(connectionId, "RECEIPT\nreceipt-id:" + receipt + "\n\n");
    }

    String username = ((UserMeneger) connections).getUsernameByConnectionId(connectionId);
    if (username != null) {
        userMeneger.logout(username);
    }

    connections.disconnect(connectionId);
    shouldTerminate = true;
    }

    private void handleError(String errorMessage) {
        connections.send(connectionId, "ERROR\nmessage:" + errorMessage + "\n\n");
        shouldTerminate = true;
    }

    private String getHeader(String[] lines, String header) {
        for (String line : lines) {
            if (line.startsWith(header + ":")) {
                return line.substring((header + ":").length());
            }
        }
        return null;
    }

    private String getBody(String[] lines) {
        boolean isBody = false;
        StringBuilder body = new StringBuilder();

        for (String line : lines) {
            if (isBody) {
                body.append(line).append("\n");
            }
            if (line.isEmpty()) {
                isBody = true;
            }
        }
        return body.toString().trim();
    }

}