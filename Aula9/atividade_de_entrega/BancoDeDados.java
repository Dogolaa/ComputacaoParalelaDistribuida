import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class BancoDeDados {
    private final Map<String, Veiculo> veiculos = new ConcurrentHashMap<>();

    public boolean inserir(Veiculo veiculo) {
        if (veiculos.containsKey(veiculo.getPlaca())) {
            return false;
        }
        veiculos.put(veiculo.getPlaca(), veiculo);
        return true;
    }

    public boolean remover(String placa) {
        return veiculos.remove(placa) != null;
    }

    public Veiculo buscar(String placa) {
        return veiculos.get(placa);
    }

    public boolean alterar(Veiculo veiculo) {
        if (!veiculos.containsKey(veiculo.getPlaca())) {
            return false;
        }
        veiculos.put(veiculo.getPlaca(), veiculo);
        return true;
    }

    public List<Veiculo> listarTodos() {
        return new ArrayList<>(veiculos.values());
    }
}