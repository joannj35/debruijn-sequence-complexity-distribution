# Complexity 71

This branch focuses on the computation of De Bruijn sequences for **complexity 71 and span 7 in a binary field**. Our approach here is tailored *specifically* for this complexity level, differing from the strategies we employed for other complexities throughout our study.

In previous complexities, our sequences could begin with any binary word (of span 7), and we typically *excluded* rotations of sequences of smaller complexities, as they result in *identical* De Bruijn sequences. However, for complexity 71, we have adopted a distinct approach due to the unique characteristics of this level.

**We initiate all De Bruijn sequences with the word `0000000`**. This choice is about adapting our methodology to the specific demands of complexity 71. By doing so, we avoid the computational intensity of considering sequences that start with different words, which is particularly crucial given the substantial number of sequences at this complexity.

Furthermore, instead of excluding smaller complexity sequences, we iterate through each one. This comprehensive and systematic method allows us to effectively manage the computational challenges posed by the large volume of sequences at complexity 71, ensuring a thorough and efficient calculation process.
