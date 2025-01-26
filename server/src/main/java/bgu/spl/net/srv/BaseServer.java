package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.impl.stomp.ConnectionsImpl;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Supplier;

public abstract class BaseServer<T> implements Server<T> {

    private final int port;
    private final Supplier<MessagingProtocol<T>> protocolFactory;
    private final Supplier<MessageEncoderDecoder<T>> encdecFactory;
    private ServerSocket sock;
    private final AtomicInteger connectionCounter = new AtomicInteger(0); // Local counter

    public BaseServer(
            int port,
            Supplier<MessagingProtocol<T>> protocolFactory,
            Supplier<MessageEncoderDecoder<T>> encdecFactory) {

        this.port = port;
        this.protocolFactory = protocolFactory;
        this.encdecFactory = encdecFactory;
		this.sock = null;
    }
    
    public void serve() {
        try (ServerSocket serverSock = new ServerSocket(port)) {
            System.out.println("Server started");

            while (!Thread.currentThread().isInterrupted()) {
                Socket clientSock = serverSock.accept();
                int connectionId = connectionCounter.incrementAndGet(); // Generate unique connectionId

                MessagingProtocol<T> protocol = protocolFactory.get();
                protocol.start(connectionId, ConnectionsImpl.getInstance()); // Initialize protocol with connectionId

                BlockingConnectionHandler<T> handler = new BlockingConnectionHandler<>(
                    clientSock,
                    encdecFactory.get(),
                    protocol
                );
                ConnectionsImpl.getInstance().addConnection((ConnectionHandler<Object>)handler, connectionId);

                execute(handler);
            }
        } 
        catch (IOException e) {
            e.printStackTrace();
        }
    }
    @Override
    public void close() throws IOException {
		if (sock != null)
			sock.close();
    }

    protected abstract void execute(BlockingConnectionHandler<T>  handler);

}
