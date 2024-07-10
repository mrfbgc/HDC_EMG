**README**

**Overview**

This project is a SystemC module for generating and binding hypervectors. The SystemC module, named binding, generates random hypervectors, performs binding operations on them, and outputs the results. It supports both binary and bipolar hypervectors.

**Files**

- item_mem.cpp: The main SystemC file containing the binding module definition and the simulation setup.

**Key Components**

**Hypervectors**

Hypervectors are high-dimensional vectors used in various computational tasks. In this project, hypervectors can be either binary (values 0 and 1) or bipolar (values -1 and 1).

**Signals and Ports**

- init, training, test: Control signals for the module.
- hv\_port1, hv\_port2: Vectors of signals for hypervector ports.

**Item Memories**

- item_memory: Stores a random hypervector for encoding feature position information.
- continuous_item_memory: Stores a random hypervector for quantizing feature value information.

**Binding Operations**

- perform_xor_operation(): Performs XOR operation for binary hypervectors.
- perform_pointwise_multiplication(): Performs pointwise multiplication for bipolar hypervectors.

**Flag**
**
- is\_binary : is the flag in order to decide binary or bipolar hypervector

**Tracing**

- The values of the control signals and hypervector ports are traced and saved in a VCD file named traces.vcd.

**Dependencies**

- SystemC library

**To-Do**

- Bundling
- Shifting
- Associative Memory
- Similarity Check Functions (cosine similartiy and hamming distance)

**Author**

Mehmet Arif Bagci – mehmet.a.bagc@fau.de
