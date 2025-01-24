package bgu.spl.net.impl.stomp;
import java.util.concurrent.ConcurrentHashMap;
import bgu.spl.net.api.*;
import bgu.spl.net.srv.BaseServer;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    //private int connectionId;
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
        //this.connectionId = connectionId;
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
        System.out.println("enter handle connect");

        String username = getHeader(lines, "login");
        String password = getHeader(lines, "passcode");
    
        if (username == null || password == null) {
            handleError("Missing login or passcode in CONNECT frame.");
            return;
        }
    
        boolean loginSuccessful = userManeger.login(username, password, BaseServer.count.get());
    
        if (loginSuccessful) {
            System.out.println("Connected");
            connections.send(BaseServer.count.get(), "CONNECTED\nversion:1.2\n\n");
        } else {
          //  String errorMsg= 
            handleError("Login failed: User already logged in or incorrect credentials.");
        }    
    }

    private void handleSubscribe(String[] lines) {
        String destination = getHeader(lines, "destination");
        String subscriptionId = getHeader(lines, "id");
        String receipt = getHeader(lines, "receipt"); // Optional receipt header
    

           // subscriptions.put(destination, Integer.parseInt(subscriptionId));
            connections.subscribe(Integer.parseInt(subscriptionId), destination);
            if (receipt != null) {
                connections.send( BaseServer.count.get(), "RECEIPT\nreceipt-id:" + receipt + "\n\n");
            }
        
          //  handleError("user already subscribe to channel "+destination);               
        
    }

    private void handleUnsubscribe(String[] lines) {
        String subscriptionId = getHeader(lines, "id");
        String receipt = getHeader(lines, "receipt"); // Optional receipt header
    
        if (subscriptionId != null) {
    
    //        subscriptions.values().remove(Integer.parseInt(subscriptionId));
            connections.unsubscribe(BaseServer.count.get(), subscriptionId);
            if (receipt != null) {
                connections.send(BaseServer.count.get(), "RECEIPT\nreceipt-id:" + receipt + "\n\n");
            }
        } else {
            handleError("Invalid UNSUBSCRIBE frame: Missing 'id' header.");
        }
    }

    private void handleSend(String[] lines) {
        String destination = getHeader(lines, "destination");
        destination = destination.substring(1); //for destination without /
        String body = getBody(lines);
        
        if (destination != null) {
            connections.send(destination, body);
        } else {
            
            handleError(userManeger.getErrorMessage());
        }
    }

    private void handleDisconnect(String[] lines) {
        String receipt = getHeader(lines, "receipt");

        if (receipt != null) {
            connections.send( BaseServer.count.get(), "RECEIPT\nreceipt-id:" + receipt + "\n\n");
        }

        String username = userManeger.getUsernameByConnectionId( BaseServer.count.get());
        if (username != null) {
            userManeger.logout(username);
        }

        connections.disconnect( BaseServer.count.get());
       // shouldTerminate = true;
    }

    private void handleError(String errorMessage) {
        connections.send( BaseServer.count.get(), "ERROR\nmessage:" + errorMessage + "\n\n");
        //shouldTerminate = true;
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
   public String createFrameMessage(String subscriptidDEST, String body)
   {
        MessageId++;
        String frame="";
        frame+= "MESSAGE\n"+
        "subscription:" + ""+"\n"
        +"message-id:"+ MessageId +"\n"+
        "destination:"+subscriptidDEST+"\n"+body+"\n"+"\n";

        return frame;
   }

}