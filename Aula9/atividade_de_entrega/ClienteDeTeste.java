import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class ClienteDeTeste {

    // ========== CONFIGURAÇÕES DO TESTE ==========
    private static final String HOST = "127.0.0.1";
    private static final int PORTA = 54323;
    private static final int NUM_THREADS = 50;
    private static final int REQUESTS_PER_THREAD = 200;
    private static final int TAMANHO_BD_INICIAL = 2000; // Quantidade de veículos a serem inseridos antes do teste
    // ==========================================

    private static final AtomicInteger SUCESSOS = new AtomicInteger(0);
    private static final AtomicInteger FALHAS = new AtomicInteger(0);
    private static final AtomicLong TEMPO_TOTAL_RESPOSTA = new AtomicLong(0);

    // Lista thread-safe para armazenar as placas que sabemos que existem no BD
    private static final List<String> placasInseridas = Collections.synchronizedList(new ArrayList<>());

    public static void main(String[] args) throws InterruptedException {
        // --- FASE 1: AQUECIMENTO / POPULAR O BANCO DE DADOS ---
        System.out.printf("Fase de Aquecimento: Inserindo %d veículos no banco de dados...%n", TAMANHO_BD_INICIAL);
        popularBancoDeDados();
        System.out.println("Aquecimento concluído.");
        System.out.println("---------------------------------------------------------");

        // --- FASE 2: TESTE DE CARGA ---
        System.out.println("Iniciando teste de carga no servidor...");
        System.out.printf("Configuração: %d clientes simultâneos, %d requisições por cliente.%n", NUM_THREADS, REQUESTS_PER_THREAD);
        System.out.println("---------------------------------------------------------");
        
        ExecutorService pool = Executors.newFixedThreadPool(NUM_THREADS);
        long testeStartTime = System.currentTimeMillis();

        for (int i = 0; i < NUM_THREADS; i++) {
            pool.submit(new ClienteWorker());
        }

        pool.shutdown();
        pool.awaitTermination(10, TimeUnit.MINUTES);
        long testeEndTime = System.currentTimeMillis();

        exibirResultados(testeEndTime - testeStartTime);
    }

    private static void popularBancoDeDados() {
        // Usamos uma única thread para inserir os dados iniciais de forma controlada
        try (Socket socket = new Socket(HOST, PORTA);
             ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
             ObjectInputStream in = new ObjectInputStream(socket.getInputStream())) {
            
            for (int i = 0; i < TAMANHO_BD_INICIAL; i++) {
                String placa = "POP-" + i;
                Veiculo v = new Veiculo(placa, "MarcaPop", "ModeloPop", 2024, "Branco", 1, 10000);
                Requisicao req = new Requisicao(Operacao.INSERIR, v);
                out.writeObject(req);
                out.flush();
                in.readObject(); // Apenas lê a resposta para sincronizar, mas não a processa aqui
                placasInseridas.add(placa);
            }
        } catch (Exception e) {
            System.err.println("Falha ao popular o banco de dados: " + e.getMessage());
        }
    }

    private static void exibirResultados(long duracaoTotalMs) {
        int totalRequisicoes = SUCESSOS.get() + FALHAS.get();
        double duracaoTotalSegundos = duracaoTotalMs / 1000.0;
        double requisicoesPorSegundo = (totalRequisicoes > 0 && duracaoTotalSegundos > 0) ? (totalRequisicoes / duracaoTotalSegundos) : 0;
        double tempoMedioRespostaMs = (totalRequisicoes > 0) ? ((TEMPO_TOTAL_RESPOSTA.get() / 1_000_000.0) / totalRequisicoes) : 0;

        System.out.println("\n------------------ RESULTADOS -------------------");
        System.out.printf("Tempo total do teste: %.2f segundos%n", duracaoTotalSegundos);
        System.out.println("Total de requisições enviadas: " + totalRequisicoes);
        System.out.println("  - Sucessos: " + SUCESSOS.get());
        System.out.println("  - Falhas: " + FALHAS.get());
        System.out.printf("Requisições por segundo (Throughput): %.2f req/s%n", requisicoesPorSegundo);
        System.out.printf("Tempo médio de resposta (Latência): %.4f ms%n", tempoMedioRespostaMs);
        System.out.println("-------------------------------------------------");
    }

    static class ClienteWorker implements Runnable {
        private final Random random = new Random();

        @Override
        public void run() {
            try (Socket socket = new Socket(HOST, PORTA);
                 ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
                 ObjectInputStream in = new ObjectInputStream(socket.getInputStream())) {

                for (int i = 0; i < REQUESTS_PER_THREAD; i++) {
                    Requisicao req = criarRequisicaoAleatoria();
                    long reqStartTime = System.nanoTime();
                    
                    out.writeObject(req);
                    out.flush();
                    Resposta res = (Resposta) in.readObject();

                    long reqEndTime = System.nanoTime();

                    if (res != null && res.getStatus() == Resposta.StatusResposta.SUCESSO) {
                        SUCESSOS.incrementAndGet();
                    } else {
                        // Mesmo que uma busca não encontre (o que não deveria acontecer agora), contamos como falha.
                        FALHAS.incrementAndGet();
                    }
                    TEMPO_TOTAL_RESPOSTA.addAndGet(reqEndTime - reqStartTime);
                }
            } catch (Exception e) {
                FALHAS.incrementAndGet();
            }
        }

        private Requisicao criarRequisicaoAleatoria() {
            if (!placasInseridas.isEmpty()) {
                String placaExistente = placasInseridas.get(random.nextInt(placasInseridas.size()));
                return new Requisicao(Operacao.BUSCAR, placaExistente);
            } else {
                return new Requisicao(Operacao.BUSCAR, "PLACA-FALHA");
            }
        }
    }
}