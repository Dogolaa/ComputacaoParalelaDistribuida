package org.example;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class OlaImpl extends UnicastRemoteObject implements Ola {

    private static final Long serialVersionUID = 1L;

    protected OlaImpl() throws RemoteException {
        super();
    }

    @Override
    public String digaOla(String nome) throws RemoteException {
        return "Ola, " + nome + "!";
    }
}
