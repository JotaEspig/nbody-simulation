pwd=$(pwd)
cd /tmp
git clone -b v0.6.5 https://github.com/JotaEspig/axolote-engine.git
cd axolote-engine
sed -i 's/\r//' install.sh
echo -n "1" | ./install.sh $pwd
cd $pwd
