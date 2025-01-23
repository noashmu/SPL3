package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("you must supply two arguments: host, message");
            System.exit(1);
        }

        int port;
        try {
            port = Integer.parseInt(args[0]);
        } catch (NumberFormatException e) {
            System.err.println("Invalid port number: " + args[0]);
            System.exit(1);
            return;
        }

        String serverType = args[1];

        switch (serverType.toLowerCase()) {
            case "tpc":
                Server.threadPerClient(
                        port,
                        () -> new StompMessagingProtocolImpl(), 
                        StompFrameEncoderDecoder::new 
                ).serve();
                break;

            case "reactor":
                Server.reactor(
                        Runtime.getRuntime().availableProcessors(),
                        port,
                        () -> new StompMessagingProtocolImpl(), 
                        StompFrameEncoderDecoder::new 
                ).serve();
                break;

            default:
                System.err.println("Unknown server type: " + serverType);
                System.exit(1);
        }
    }
}
