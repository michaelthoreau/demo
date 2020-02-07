# demo

## install git and python3 (ubuntu)
```
sudo apt install git
```

## Setup your git
```
git config --global user.email <your email>
git config --global user.name
```


## clone the repo
```
git clone https://github.com/michaelthoreau/demo.git
```

## lets check the status of our repo
```
cd demo
git status
```

## make a new file
```
echo "print(\"test\")" > test.py
python3 test.py
```

## lets see the new status
```
git status
```

## ok so we have an **untracked file**
```
# add the file to the staging area
git add test.py

# see the updated status
git status
```

## now commit to the local repository
```
# commit
git commit -m "my commit message"

# see the updated status
git status
```

## now we push the local repository to the cloud (remote)
```
git push
```

