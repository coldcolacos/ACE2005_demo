# ACE2005_demo


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
