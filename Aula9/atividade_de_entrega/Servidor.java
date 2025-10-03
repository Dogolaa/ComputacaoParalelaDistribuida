import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Servidor {
    private static final int PORTA = 54323;

    public static void main(String[] args) {
        BancoDeDados bancoDeDados = new BancoDeDados();

        try (ServerSocket serverSocket = new ServerSocket(PORTA)) {
            System.out.println("Servidor iniciado. Aguardando clientes na porta " + PORTA + "...");

            while (true) {
                Socket clienteSocket = serverSocket.accept();
                System.out.println("Novo cliente conectado: " + clienteSocket.getInetAddress().getHostAddress());

                GerenciadorCliente gerenciador = new GerenciadorCliente(clienteSocket, bancoDeDados);
                new Thread(gerenciador).start();
            }
        } catch (IOException e) {
            System.err.println("Erro no servidor: " + e.getMessage());
        }
    }
}