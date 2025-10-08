import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.List;
import java.util.Scanner;

public class Cliente {
    private static final String HOST = "127.0.0.1";
    private static final int PORTA = 54323;

    public static void main(String[] args) {
        try (
            Socket socket = new Socket(HOST, PORTA);
            ObjectOutputStream saida = new ObjectOutputStream(socket.getOutputStream());
            ObjectInputStream entrada = new ObjectInputStream(socket.getInputStream());
            Scanner scanner = new Scanner(System.in)
        ) {
            System.out.println("Conectado ao servidor!");
            int opcao;
            do {
                exibirMenu();
                opcao = Integer.parseInt(scanner.nextLine());
                switch (opcao) {
                    case 1:
                        inserirVeiculo(scanner, saida, entrada);
                        break;
                    case 2:
                        removerVeiculo(scanner, saida, entrada);
                        break;
                    case 3:
                        buscarVeiculo(scanner, saida, entrada);
                        break;
                    case 4:
                        alterarVeiculo(scanner, saida, entrada);
                        break;
                    case 5:
                        listarTodos(saida, entrada);
                        break;
                    case 0:
                        System.out.println("Saindo...");
                        break;
                    default:
                        System.out.println("Opção inválida!");
                }
            } while (opcao != 0);

        } catch (Exception e) {
            System.err.println("Erro no cliente: " + e.getMessage());
        }
    }

    private static void exibirMenu() {
        System.out.println("\n--- Sistema de Gestão de Veículos ---");
        System.out.println("1. Inserir Veículo");
        System.out.println("2. Remover Veículo");
        System.out.println("3. Buscar Veículo por Placa");
        System.out.println("4. Alterar Veículo");
        System.out.println("5. Listar Todos os Veículos");
        System.out.println("0. Sair");
        System.out.print("Escolha uma opção: ");
    }

    private static void enviarRequisicao(Requisicao req, ObjectOutputStream out, ObjectInputStream in) throws Exception {
        out.writeObject(req);
        out.flush();
        Resposta resposta = (Resposta) in.readObject();
        System.out.println("Resposta do Servidor: " + resposta.getStatus());
        
        if (resposta.getCorpo() instanceof List) {
             ((List<?>) resposta.getCorpo()).forEach(System.out::println);
        } else {
            System.out.println(resposta.getCorpo());
        }
    }
    
    private static void inserirVeiculo(Scanner sc, ObjectOutputStream out, ObjectInputStream in) throws Exception {
        System.out.println("--- Inserir Veículo ---");
        System.out.print("Placa: ");
        String placa = sc.nextLine();
        System.out.print("Marca: ");
        String marca = sc.nextLine();
        System.out.print("Modelo: ");
        String modelo = sc.nextLine();
        System.out.print("Ano: ");
        int ano = Integer.parseInt(sc.nextLine());
        System.out.print("Cor: ");
        String cor = sc.nextLine();
        System.out.print("KM: ");
        double km = Double.parseDouble(sc.nextLine());
        System.out.print("Valor: ");
        double valor = Double.parseDouble(sc.nextLine());

        Veiculo v = new Veiculo(placa, marca, modelo, ano, cor, km, valor);
        enviarRequisicao(new Requisicao(Operacao.INSERIR, v), out, in);
    }

    private static void removerVeiculo(Scanner sc, ObjectOutputStream out, ObjectInputStream in) throws Exception {
        System.out.print("Digite a placa do veículo a ser removido: ");
        String placa = sc.nextLine();
        enviarRequisicao(new Requisicao(Operacao.REMOVER, placa), out, in);
    }
    
    private static void buscarVeiculo(Scanner sc, ObjectOutputStream out, ObjectInputStream in) throws Exception {
        System.out.print("Digite a placa do veículo a ser buscado: ");
        String placa = sc.nextLine();
        enviarRequisicao(new Requisicao(Operacao.BUSCAR, placa), out, in);
    }
    
    private static void alterarVeiculo(Scanner sc, ObjectOutputStream out, ObjectInputStream in) throws Exception {
        System.out.println("--- Alterar Veículo ---");
        System.out.print("Digite a placa do veículo que deseja alterar: ");
        String placa = sc.nextLine();
        System.out.print("Nova Marca: ");
        String marca = sc.nextLine();
        System.out.print("Novo Modelo: ");
        String modelo = sc.nextLine();
        System.out.print("Novo Ano: ");
        int ano = Integer.parseInt(sc.nextLine());
        System.out.print("Nova Cor: ");
        String cor = sc.nextLine();
        System.out.print("Nova KM: ");
        double km = Double.parseDouble(sc.nextLine());
        System.out.print("Novo Valor: ");
        double valor = Double.parseDouble(sc.nextLine());
        
        Veiculo v = new Veiculo(placa, marca, modelo, ano, cor, km, valor);
        enviarRequisicao(new Requisicao(Operacao.ALTERAR, v), out, in);
    }

    private static void listarTodos(ObjectOutputStream out, ObjectInputStream in) throws Exception {
         enviarRequisicao(new Requisicao(Operacao.LISTAR_TODOS, null), out, in);
    }
}