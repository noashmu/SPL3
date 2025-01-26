package bgu.spl.net.impl.stomp;
import java.util.concurrent.ConcurrentLinkedQueue;

import bgu.spl.net.api.*;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    private int connectionId;
    private Connections<String> connections=ConnectionsImpl.getInstance();
    private boolean shouldTerminate = false;
    private UserManeger userManeger;  // Reference to UserManager
    private int MessageId;


    // Store user subscriptions
   // private ConcurrentHashMap<String, Integer> subscriptions = new ConcurrentHashMap<>();

    public StompMessagingProtocolImpl(){
        userManeger = new UserManeger();
        MessageId=0;
    }

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId=connectionId;
        this.connections = connections;
    }
    

    @Override
    public String process(String message) {
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

        return null; ///////////////maybe need to change///////////////
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
    
        boolean loginSuccessful = userManeger.login(username, password, connectionId);
    
        if (loginSuccessful) {
            connections.send(connectionId, "CONNECTED\nversion:1.2\n\n");
        } else {
            handleError("Login failed: User already logged in or incorrect credentials.");
        }    
    }

    private void handleSubscribe(String[] lines) {
        String destination = getHeader(lines, "destination");
        String receipt = getHeader(lines, "receipt"); // Optional receipt header
    

           // subscriptions.put(destination, Integer.parseInt(subscriptionId));
            connections.subscribe(connectionId, destination);
            if (receipt != null) {
                connections.send( connectionId, "RECEIPT\nreceipt-id:" + receipt + "\n\n");
            }
        
          //  handleError("user already subscribe to channel "+destination);               
        
    }

    private void handleUnsubscribe(String[] lines) {
        String subscriptionId = getHeader(lines, "id");
        String receipt = getHeader(lines, "receipt"); // Optional receipt header
    
        if (subscriptionId != null) {
    
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
        String frame=createFrameMessage(destination,body);
        if (destination != null) {
            if (((ConnectionsImpl)connections).IsSubcribers(destination)) {
                connections.send(destination, frame);
            }
            else
            {
                handleError("The channel doesn't have subscribers");
            }
        } else {
            handleError("The channel doesn't exist");
        }
    }

    private void handleDisconnect(String[] lines) {
        String receipt = getHeader(lines, "receipt");

        if (receipt != null) {
            connections.send(connectionId, "RECEIPT\nreceipt-id:" + receipt + "\n\n");
        }

        String username = userManeger.getUsernameByConnectionId(connectionId);
        if (username != null) {
            userManeger.logout(username);
        }

        connections.disconnect(connectionId);
    }

    private void handleError(String errorMessage) {
        connections.send(connectionId, "ERROR\nmessage:" + errorMessage + "\n\n");
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
   public Connections<String> getConnections()
   {
    return this.connections;
   }
   public String createFrameMessage(String destinition, String body)
   {
        MessageId++;
        String frame="";
        frame+= "MESSAGE\n"+
        "subscription:" + connectionId+"\n"
        +"message-id:"+ MessageId +"\n"+
        "destination:"+destinition+"\n"+body+"\n"+"\n";

        return frame;
   }

}