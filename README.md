# ACE2005_demo

## Download

(1) This dataset is not free.

https://catalog.ldc.upenn.edu/LDC2006T06

(2) Latest stanfordcorenlp.

http://nlp.stanford.edu/software/stanford-corenlp-full-2018-10-05.zip


## Preprocess

(1) Decompress all __gz__ files using command below.

```
for gz in *.gz; do gunzip $gz; done
```

(2) Split by stanfordcorenlp, CentOS is OK but some trouble for macOS.

(3) Resource punkt not found.

```
>>> import nltk
>>> nltk.download('punkt')
```
