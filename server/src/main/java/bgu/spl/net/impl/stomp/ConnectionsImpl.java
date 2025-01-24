package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.BaseServer;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImpl<T> implements Connections<T>{
    private final ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> topicSubscribers = new ConcurrentHashMap<>();
    private static ConnectionsImpl<?> INSTANCE;
    private ConnectionsImpl(){

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
        System.out.println(connectionId);
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
    }

    @Override
    public void disconnect(int connectionId) {
        connectionHandlers.remove(connectionId);
        topicSubscribers.forEach((channel, subscribers) -> subscribers.remove(connectionId));
    }

    @Override
    public void subscribe(int connectionId, String channel) {
        topicSubscribers.computeIfAbsent(channel, k -> new ConcurrentLinkedQueue<>()).add(connectionId);
    }

    @Override
    public void unsubscribe(int connectionId, String channel) {
        ConcurrentLinkedQueue<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            subscribers.remove(connectionId);
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
        connectionHandlers.put(BaseServer.count.get(), handler);

    }

}
