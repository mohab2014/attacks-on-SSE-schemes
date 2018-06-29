# attacks-on-SSE-schemes
Practical Attacks on Relational Databases Protected via Searchable Encryption


Searchable symmetric encryption (SSE) schemes are commonly proposed to enable search in a protected unstructured documents such as email archives or any set of sensitive text files. 

However, some SSE schemes have been recently proposed in order to protect relational databases. Most of the previous attacks on SSE schemes have only targeted its common use case, protecting unstructured data. 

We propose here an inference attack that enables a passive adversary with only basic knowledge about the meta-data information of the target relational database to recover the attribute names of some observed queries. This violates query privacy since the attribute name of a query is secret.


Our attack is implemented using several approaches to recover the attribute names of observed queries:

1. Subset sum approach.
2. Independent set approach.
3. Eficient heuristic approach that performs intersection between the search results of observed queries.
4. Combining the subset sum approach with the heuristic approach.

For more information, refer to the paper [1]. Our attacks are validated on three real world datasets: Adult [2], Bank [3] and the Census [4] datasets.


1. Mohamed Ahmed Abdelraheem, Tobias Andersson, Christian Gehrmann and Cornelius Glackin. Practical Attacks on Relational Databases Protected via Searchable Encryption. ISC 2018. 
2. R. Kohavi and B. Becker. Adult data set. https://archive.ics.uci.edu/ml/machine-learning-databases/adult/, 1996. [Last Accessed June 2018].
3. R. Laureano S. Moro and P. Cortez. Using data mining for bank direct marketing: An application of the crisp-dm methodology. In P. Novais et al. (Eds.), Proceedings of the European Simulation and Modelling Conference - ESM’2011, pp. 117-121,
Guimarães, Portugal, October, 2011. EUROSIS, https://archive.ics.uci.edu/ml/datasets/Bank+Marketing. [Last Accessed June 2018].
4. Terran Lane and Ronny Kohavi. Census-income (kdd) data set. https://archive.ics.uci.edu/ml/machine-learning-databases/census-income-mld/, 2000. [Last Accessed June 2018].






