import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.util.HashSet;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.net.InetAddress;

class Receiver {

    static byte[] receiveData; // buffer for received UDP packets
    static DatagramSocket serverSocket;

    public static void main(String args[]) throws Exception {
        System.out.println("Receiver starting at: " + System.currentTimeMillis());

        int portNo = Integer.parseInt(args[0]); // port number to listen to packets on
        int maxPacket = Integer.parseInt(args[1]); // number of packets to listen for

        serverSocket = new DatagramSocket(portNo);
        receiveData = new byte[1024];

        String packet;

        int count = 0;
        while (count != maxPacket ) {
            count++;
            if (count % 100 == 0) {
                System.out.println("Received " + count + " packets");
            }

            // receive a packet
            Arrays.fill(receiveData, (byte) 0);

            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            serverSocket.receive(receivePacket);
            packet = new String( receivePacket.getData());

        }

        long endTime = System.nanoTime();

        System.out.println("Receiver ending at: " + System.currentTimeMillis());


    }
}