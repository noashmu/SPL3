package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

public class UserMeneger {
    private final ConcurrentHashMap<String, String> users = new ConcurrentHashMap<>(); // username -> password
    private final ConcurrentHashMap<Integer, String> activeUsers = new ConcurrentHashMap<>(); // connectionId -> username

    // Log in a user
    public synchronized boolean login(String username, String password, int connectionId) {
        if (!users.containsKey(username)) {
            users.put(username, password); // Register new user
        }
        if (users.get(username).equals(password) && !activeUsers.containsKey(username)) {
            activeUsers.put(connectionId, username);
            return true;
        }
        return false;
    }

    // Log out a user
    public synchronized void logout(String username) {
        activeUsers.values().remove(username);
    }

    // Get username by connection ID
    public String getUsernameByConnectionId(int connectionId) {
        return activeUsers.get(connectionId);
    }
}
