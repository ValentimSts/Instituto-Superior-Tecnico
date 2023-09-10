# SD
SD - Sistemas Distribuídos (Distributed Systems).

---

A ledger like application with client, admin and server side programs that communicate via GRPC. Multiple servers can run at the same time (replicas), that use a gossip architecture to maintain consistency between replicas while also providing a high availability for the clients.  