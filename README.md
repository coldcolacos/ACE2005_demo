# ACE2005_demo


## preprocess

(1) All files are in *.gz format, decompress command is

```
for gz in *.gz; do gunzip $gz; done
```

(2) split by stanfordcorenlp, CentOS is OK but some trouble for macOS

