# Attacks on Searchable Symmetric Encryption (SSE) Schemes
Practical Attacks on Relational Databases Protected via Searchable Encryption


Searchable symmetric encryption (SSE) schemes are commonly proposed to enable search in a protected unstructured documents such as email archives or any set of sensitive text files. 

However, some SSE schemes have been recently proposed in order to protect relational databases. Most of the previous attacks on SSE schemes have only targeted its common use case, protecting unstructured data. Our work improves over the recent attacks proposed in [1] and can be accessed from the link https://github.com/mohab2014/Searchable-Encrypted-Relational-Databases.

We propose here an inference attack that enables a passive adversary with only basic knowledge about the meta-data information of the target relational database to recover the attribute names of some observed queries. This violates query privacy since the attribute name of a query is secret.


Our attack is implemented using several approaches to recover the attribute names of observed queries:

1. Subset sum approach.
2. Independent set approach.
3. Eficient heuristic approach that performs intersection between the search results of observed queries.
4. Combining the subset sum approach with the heuristic approach.


For more information, refer to the paper [2]. Our attacks are validated on three real world datasets: Adult [3], Bank [4] and the Census [5] datasets.

1. Mohamed Ahmed Abdelraheem, Tobias Andersson, Christian Gehrmann. Searchable Encrypted Relational Databases: Risks and   Countermeasures. DPM 2017. https://eprint.iacr.org/2017/024.pdf
2. Mohamed Ahmed Abdelraheem, Tobias Andersson, Christian Gehrmann and Cornelius Glackin. Practical Attacks on Relational Databases Protected via Searchable Encryption. ISC 2018. 
3. R. Kohavi and B. Becker. Adult data set. https://archive.ics.uci.edu/ml/machine-learning-databases/adult/, 1996. [Last Accessed June 2018].
4. R. Laureano S. Moro and P. Cortez. Using data mining for bank direct marketing: An application of the crisp-dm methodology. In P. Novais et al. (Eds.), Proceedings of the European Simulation and Modelling Conference - ESM’2011, pp. 117-121,
Guimarães, Portugal, October, 2011. EUROSIS, https://archive.ics.uci.edu/ml/datasets/Bank+Marketing. [Last Accessed June 2018].
5. Terran Lane and Ronny Kohavi. Census-income (kdd) data set. https://archive.ics.uci.edu/ml/machine-learning-databases/census-income-mld/, 2000. [Last Accessed June 2018].






