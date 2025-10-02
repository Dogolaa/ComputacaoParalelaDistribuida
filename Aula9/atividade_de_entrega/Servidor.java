import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Servidor {
    private static final int PORTA = 54323;

    public static void main(String[] args) {
        BancoDeDados bancoDeDados = new BancoDeDados(); // Instância única do BD

        try (ServerSocket serverSocket = new ServerSocket(PORTA)) {
            System.out.println("Servidor iniciado. Aguardando clientes na porta " + PORTA + "...");

            // Fase 2: Loop infinito para aceitar múltiplos clientes
            while (true) {
                Socket clienteSocket = serverSocket.accept();
                System.out.println("Novo cliente conectado: " + clienteSocket.getInetAddress().getHostAddress());

                // Cria uma nova thread para lidar com o cliente
                GerenciadorCliente gerenciador = new GerenciadorCliente(clienteSocket, bancoDeDados);
                new Thread(gerenciador).start();
            }
        } catch (IOException e) {
            System.err.println("Erro no servidor: " + e.getMessage());
        }
    }
}