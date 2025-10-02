import java.io.Serializable;

public class Requisicao implements Serializable {
    private static final long serialVersionUID = 1L;

    private Operacao operacao;
    private Object corpo;

    public Requisicao(Operacao operacao, Object corpo) {
        this.operacao = operacao;
        this.corpo = corpo;
    }

    public Operacao getOperacao() {
        return operacao;
    }

    public Object getCorpo() {
        return corpo;
    }
}