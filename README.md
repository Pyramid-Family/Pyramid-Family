# Pyramid Family: Generic Frameworks for Accurate and Fast Flow Size Measurement


## Introduction
Sketches, as a kind of probabilistic data structures, have been considered as the most promising solution for network measurement in recent years. Most sketches do not work well for skewed network traffic. To address this problem, we propose a family of sketch frameworks, namely the Pyramid family. The first member of our Pyramid family is the S-Pyramid framework, which includes two techniques: counter-pair sharing for high accuracy, and word acceleration for fast speed. The second member of our Pyramid family is the Mini-Pyramid framework, which projects the S-Pyramid framework into one counter, bringing more flexibility in application while keeping the accuracy. To demonstrate the generality of our Pyramid family, we apply both frameworks to sketches of CM, CU, Count, and Augmented. To demonstrate the flexibility of the Mini-Pyramid framework, we further apply Mini-Pyramid to SBF and the On-Off sketch. The experimental results show that, the S-Pyramid framework can reduce the ARE by up to 7.12 times compared with the original sketches, while improving the throughput by up to 2.37 times; the Mini-Pyramid framework can reduce the ARE by up to 29.2 times, at the cost of 21.3% lower throughput on average.


## Repository Structure
*  `Classic/`: the source code of S-Pyramid and Mini-Pyramid applying to classic sketches (CM, CU, Count, and Asketch)
*  `SBF/`: the source code of Mini-Pyramid applying to SBF
*  `On-Off/`: the source code of Mini-Pyramid applying to On-Off


## Requirements
- cmake
- g++


## Datasets
The dataset files should be binary files containing only flow IDs.
Each flow ID is a fixed-length string, whose length should be equal to KEY_LENGTH defined in the code.
Flow ID is commonly defined as srcIP, <srcIP, dstIP>, or 5-tuple (srcIP, srcPort, dstIP, dstPort, protocol).


## Publication
[(VLDB17) Pyramid Sketch: a Sketch Framework for Frequency Estimation of Data Streams](https://dl.acm.org/doi/abs/10.14778/3137628.3137652)
