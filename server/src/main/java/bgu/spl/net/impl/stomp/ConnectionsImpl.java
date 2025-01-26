package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T>{
    private final ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    private final ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> topicSubscribers;
    private static ConnectionsImpl<?> INSTANCE;
    String errorMsg;

    private ConnectionsImpl(){
        connectionHandlers= new ConcurrentHashMap<>();
        topicSubscribers= new ConcurrentHashMap<>();
    }
    public static synchronized <T> ConnectionsImpl<T> getInstance()
    {
        if (INSTANCE==null) {
            INSTANCE= new ConnectionsImpl<>();
        }
        return (ConnectionsImpl<T>)INSTANCE;
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = connectionHandlers.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        ConcurrentLinkedQueue<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            for (Integer connectionId : subscribers) {
                send(connectionId, msg);
            }
        }
        else if (subscribers.isEmpty()) {
            this.errorMsg="no subscribers to channel or channel doessn't exist";
           }
    }

    @Override
    public void disconnect(int connectionId) {
        connectionHandlers.remove(connectionId);
        topicSubscribers.forEach((channel, subscribers) -> subscribers.remove(connectionId));
    }

    @Override
    public void subscribe(int connectionId, String channel) {
        ConcurrentLinkedQueue<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            if (subscribers != null && subscribers.contains(connectionId)) {
                System.out.println("User already subscribed to channel: " + channel);
                return; 
            }
        }
        topicSubscribers.computeIfAbsent(channel, k -> new ConcurrentLinkedQueue<>()).add(connectionId);
    }

    @Override
    public void unsubscribe(int connectionId, String channel) {
        ConcurrentLinkedQueue<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            if (subscribers.contains(connectionId)) {
                subscribers.remove(connectionId);
            }
            else{
                System.out.println("user is not subscribe to channel "+channel );
            }
        
        }
    }

    
    @Override
    public int addConnection(ConnectionHandler<T> handler) {
        int connectionId = connectionHandlers.size() + 1; // Generate a unique ID
        connectionHandlers.put(connectionId, handler);
        return connectionId;
    }
    public void addConnection(ConnectionHandler<T> handler,int connectionId)
    {
        connectionHandlers.put(connectionId ,handler);

    }
    public boolean IsSubcribers(String channel)
    {
        if (topicSubscribers==null) {
            return false;
        }
        ConcurrentLinkedQueue<Integer> subscribers = topicSubscribers.get(channel);

        if (subscribers==null || subscribers.isEmpty()) {
            return false;
        }
        return true;
    }

}
