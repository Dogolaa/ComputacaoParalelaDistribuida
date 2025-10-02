import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.List;

public class GerenciadorCliente implements Runnable {
    private final Socket clienteSocket;
    private final BancoDeDados bancoDeDados;

    public GerenciadorCliente(Socket socket, BancoDeDados bd) {
        this.clienteSocket = socket;
        this.bancoDeDados = bd;
    }

    @Override
    public void run() {
        try (
            ObjectOutputStream saida = new ObjectOutputStream(clienteSocket.getOutputStream());
            ObjectInputStream entrada = new ObjectInputStream(clienteSocket.getInputStream())
        ) {
            Requisicao requisicao;
            // Loop para processar múltiplas requisições do mesmo cliente
            while ((requisicao = (Requisicao) entrada.readObject()) != null) {
                Resposta resposta = processarRequisicao(requisicao);
                saida.writeObject(resposta);
                saida.flush();
            }
        } catch (Exception e) {
            System.out.println("Cliente " + clienteSocket.getInetAddress().getHostAddress() + " desconectado.");
        }
    }

    private Resposta processarRequisicao(Requisicao req) {
        Veiculo veiculo;
        String placa;

        switch (req.getOperacao()) {
            case INSERIR:
                veiculo = (Veiculo) req.getCorpo();
                boolean inserido = bancoDeDados.inserir(veiculo);
                if (inserido) {
                    return new Resposta(Resposta.StatusResposta.SUCESSO, "Veículo inserido com sucesso!");
                } else {
                    return new Resposta(Resposta.StatusResposta.ERRO, "Veículo com esta placa já existe.");
                }

            case REMOVER:
                placa = (String) req.getCorpo();
                boolean removido = bancoDeDados.remover(placa);
                if (removido) {
                    return new Resposta(Resposta.StatusResposta.SUCESSO, "Veículo removido com sucesso!");
                } else {
                    return new Resposta(Resposta.StatusResposta.FALHA_NAO_ENCONTRADO, "Veículo não encontrado.");
                }

            case BUSCAR:
                placa = (String) req.getCorpo();
                Veiculo encontrado = bancoDeDados.buscar(placa);
                if (encontrado != null) {
                    return new Resposta(Resposta.StatusResposta.SUCESSO, encontrado);
                } else {
                    return new Resposta(Resposta.StatusResposta.FALHA_NAO_ENCONTRADO, "Veículo não encontrado.");
                }

            case ALTERAR:
                veiculo = (Veiculo) req.getCorpo();
                boolean alterado = bancoDeDados.alterar(veiculo);
                if (alterado) {
                    return new Resposta(Resposta.StatusResposta.SUCESSO, "Veículo alterado com sucesso!");
                } else {
                    return new Resposta(Resposta.StatusResposta.FALHA_NAO_ENCONTRADO, "Veículo não encontrado para alteração.");
                }

            case LISTAR_TODOS:
                List<Veiculo> todos = bancoDeDados.listarTodos();
                return new Resposta(Resposta.StatusResposta.SUCESSO, todos);

            default:
                return new Resposta(Resposta.StatusResposta.ERRO, "Operação desconhecida.");
        }
    }
}