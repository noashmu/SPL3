package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

public class UserManeger {
    private final ConcurrentHashMap<String, String> users; // username -> password
    private final ConcurrentHashMap<Integer, String> activeUsers; // connectionId -> username
    String errorMsg="";
    
    public UserManeger(){
        users= new ConcurrentHashMap<>();
        activeUsers = new ConcurrentHashMap<>();
        errorMsg="";
    }

    // Log in a user
    public synchronized boolean login(String username, String password, int connectionId) {
        if (!users.containsKey(username)) {
            users.put(username, password); // Register new user
        }
        if (users.get(username).equals(password) && !activeUsers.containsKey(username)) {
            activeUsers.put(connectionId, username);
            return true;
        }
        if (activeUsers.contains(username)) {
            System.out.println("The client is already logged in, log out before trying again");
        }
        if (!users.get(username).equals(password)) {
            this.errorMsg="Worng password";
        }
        return false;
    }
    public String getErrorMessage()
    {
        return errorMsg;
    }

    // Log out a user
    public synchronized void logout(String username) {
        if (!activeUsers.contains(username)) {
            System.out.println("the user is not logged in");
        }
        activeUsers.values().remove(username);
    }

    // Get username by connection ID
    public String getUsernameByConnectionId(int connectionId) {
        return activeUsers.get(connectionId);
    }
}
