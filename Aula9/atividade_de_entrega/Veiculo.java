//package sockets.objeto;

import java.io.Serializable;

public class Veiculo  implements Serializable{

    private String placa;
    private String marca;
    private String modelo;
    private String ano;
    private String cor;
    private String km;
    private Double preco;

    public Veiculo(){}

    public Veiculo(String placa, String marca, String modelo, String ano, String cor, String km, Double preco) {
        this.placa = placa;
        this.marca = marca;
        this.modelo = modelo;
        this.ano = ano;
        this.cor = cor;
        this.km = km;
        this.preco = preco;
    }

    public String getPlaca() {
        return placa;
    }

    public void setPlaca(String placa) {
        this.placa = placa;
    }

    public String getMarca() {
        return marca;
    }

    public void setMarca(String marca) {
        this.marca = marca;
    }

    public String getModelo() {
        return modelo;
    }

    public void setModelo(String modelo) {
        this.modelo = modelo;
    }

    public String getAno() {
        return ano;
    }

    public void setAno(String ano) {
        this.ano = ano;
    }

    public String getCor() {
        return cor;
    }

    public void setCor(String cor) {
        this.cor = cor;
    }

    public String getKm() {
        return km;
    }

    public void setKm(String km) {
        this.km = km;
    }

    public Double getPreco() {
        return preco;
    }

    public void setPreco(Double preco) {
        this.preco = preco;
    }

    public boolean equals(Object object) {
        if (object == null || getClass() != object.getClass()) return false;
        if (!super.equals(object)) return false;
        Veiculo veiculo = (Veiculo) object;
        return java.util.Objects.equals(placa, veiculo.placa) && java.util.Objects.equals(marca, veiculo.marca) && java.util.Objects.equals(modelo, veiculo.modelo) && java.util.Objects.equals(ano, veiculo.ano) && java.util.Objects.equals(cor, veiculo.cor) && java.util.Objects.equals(km, veiculo.km) && java.util.Objects.equals(preco, veiculo.preco);
    }

    public int hashCode() {
        return Objects.hash(super.hashCode(), placa, marca, modelo, ano, cor, km, preco);
    }
}
