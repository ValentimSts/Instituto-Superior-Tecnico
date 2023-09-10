A arquitetura gossip foi baseada no livro da cadeira, com breves alterações:

- MAX_REPLICAS: Este código foi desenvolvido para ser adaptado a qualquer número de réplicas, funcionando tanto com duas (como estipulado no enunciado) como com mais, dependendo do valor máximo de réplicas definido previamente no Common file partilhado pelos servidores, user e admin (que vai determinar o tamanho dos timestamps).

- NamingServerState serverIDs: IDs oferecidos pelo naming server para identificar os servidores (réplicas)

- ServerState id: Para além dos value, replica e table timestamp, foi acrescentado um id em cada servidor, para identificar o índice dessa réplica nos timestamps (útil na propagação de operações para atualização dos timestamps).

- serverID: Caso um servidor vá a baixo (seja chamado o método delete()), o ID deste mesmo é reposto como 'available', para que outros servidores o possam usar futuramente. Contudo, as operações da ledger do servidor apagado, são perdidas, o que está fora do nosso controlo.

- ServerStateProcess (Operações de leitura): Objeto usado apara chamar wait() até que a operação seja estável.

- Operações de leitura (balance): Caso seja efetuado um balance e pedido.prev > value timestamp, o pedido fica pendente através de um wait(), que espera que seja notificado (NÃO FUNCIONA).
  - Tentamos fazer da maneira mencionada acima. No entanto, visto que não funcionou, apenas lançamos uma exceção.

- Operações de escrita (createAccount/transferTo): respondem imediatamente ao client, independentemente de serem estáveis ou não.

- Gossip: Para estimar as operações que um server propaga para outro num gossip, utilizamos a tableTS que contém a TS da operação mais recente que foi enviada a cada uma das réplica. Desta forma conseguimos não eviar operações repetidas em chamadas gossip.
