import java.io.Serializable;

public class Resposta implements Serializable {
    private static final long serialVersionUID = 1L;

    private StatusResposta status;
    private Object corpo;

    public enum StatusResposta {
        SUCESSO,
        ERRO,
        FALHA_NAO_ENCONTRADO
    }

    public Resposta(StatusResposta status, Object corpo) {
        this.status = status;
        this.corpo = corpo;
    }

    public StatusResposta getStatus() {
        return status;
    }

    public Object getCorpo() {
        return corpo;
    }
}