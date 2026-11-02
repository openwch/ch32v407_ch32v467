This section of examples mainly demonstrates the implementation and application of the dual-network-port function.

The hardware platform consists of the CH32V4x7 and the CH390D. The CH32V4x7 communicates with the CH390D through the SPI interface,
and the CH390D serves as an expansion network port and together with the Ethernet interface built into the CH32V4x7 forms a dual-network-port system.

The software protocol stack supports the two network ports running independently or working cooperatively, meeting a variety of network application requirements,
including network port cascading, IP routing/forwarding, and dual-network-port redundancy scenarios.