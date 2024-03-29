# Complexity Distribution of de Bruijn Sequence Over Prime Fields
[![DOI](https://zenodo.org/badge/653524698.svg)](https://zenodo.org/doi/10.5281/zenodo.10685234)
[![License](https://img.shields.io/badge/license-Apache_2.0-green)](https://github.com/joannj35/debruijn-sequence-complexity-distribution/blob/master/LICENSE)
[![complexity71](https://img.shields.io/badge/complexity_71-branch-orange)](https://github.com/joannj35/debruijn-sequence-complexity-distribution/tree/Binary_Span7_71)
[![tool](https://img.shields.io/badge/GUI-DSP-8A2BE2)](https://github.com/joannj35/debruijn-sequence-parser)

## Overview
In this project we continue the study of the linear complexity of de Bruijn sequences over finite fields. Our aim is to establish the integer values of
linear complexity for which there exist de Bruijn sequences of given span over finite fields. **This repository includes the code developed for calculating these complexity distributions.**

## Research Domains
Our study is divided into two distinct research domains:
  
### Binary Fields
This section examines de Bruijn sequences within binary fields (prime field 2), focusing on sequences with spans of 6 and 7. The computation of the complexities utilizes ***multithreading*** and ***OpenMP*** to efficiently handle complex calculations and data processing.

***NOTE***:
Complexity 71 of span 7 is calculated in a different approach under the branch [Binary_Span7_71](https://github.com/joannj35/debruijn-sequence-complexity-distribution/tree/Binary_Span7_71)

### Non-Binary Fields
This section examines de Bruijn sequences within non-binary fields (prime fields 3,5,7), particularly focusing on sequences with span 2.

## Additional Tools and Visualization
For a more enhanced user experience, we have developed a (humble) GUI tool, housed in a separate repository. This GUI effectively visualizes the result data, providing a more intuitive understanding of the complexity distributions. For access to the GUI and further instructions, please refer to [GUI repository](https://github.com/joannj35/debruijn-sequence-parser).

## Citation
If you use this software or the [data it produces](https://github.com/joannj35/debruijn-sequence-parser/tree/master/data) in your work,  please cite it using the following reference:

Etzion, T., Jozen, J., & Dally, R. (2024). debruijn-sequence-complexity-distribution (Version 1.0.0) [Computer software]. https://doi.org/10.5281/zenodo.10685239

## Using the Code
Each branch of this repository contains specific instructions and scripts for the respective domain. To simplify running the code, we have introduced a Command Line Interface (CLI) which prompts the user for necessary inputs, streamlining the execution process. 

### Binary Field Domain Feature
In the binary field domain ***ONLY***, users have the exclusive capability to pause and unpause complexity calculation at command. If the code is paused and later unpaused, it will resume from the exact point where it was halted, thereby conserving valuable computation time and resources.

## Contributing
Contributions to this project are highly appreciated. If you have suggestions or improvements, please review the [LICENSE file](https://github.com/joannj35/debruijn-sequence-complexity-distribution/blob/master/LICENSE) , then fork the appropriate branch of the repository, make your changes, and submit a pull request.

## Acknowledgments
Special acknowledgment to [Tuvi Etzion](https://etzion.net.technion.ac.il/), professor of computer science at **Technion – Israel Institute of Technology**, for his invaluable advice and support throughout this project. His expertise significantly contributed to the successful completion of this work

## LICENSE
This project is licensed under Apache License 2.0. For more information, see the LICENSE file in this repository.

