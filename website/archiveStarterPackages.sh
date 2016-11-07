rm *-Starter-Package.zip

cd ../airesources/

rm -r *-Starter-Package

mkdir Halite-Python-Starter-Package Halite-Java-Starter-Package Halite-C++-Starter-Package Halite-Rust-Starter-Package Halite-C#-Starter-Package Halite-Scala-Starter-Package

cp -r Python/* Halite-Python-Starter-Package/
cp -r Java/* Halite-Java-Starter-Package/
cp -r C++/* Halite-C++-Starter-Package/
cp -r Rust/* Halite-Rust-Starter-Package/
cp -r Java/* Halite-Scala-Starter-Package/
cp -r CSharp/* Halite-C#-Starter-Package/

cp -r Scala/* Halite-Scala-Starter-Package/
rm Halite-Scala-Starter-Package/MyBot.java

# Add install.sh to all zip archives
cp ../environment/install.sh Halite-Python-Starter-Package/
cp ../environment/install.sh Halite-Java-Starter-Package/
cp ../environment/install.sh Halite-C++-Starter-Package/
cp ../environment/install.sh Halite-Rust-Starter-Package/
cp ../environment/install.sh Halite-Scala-Starter-Package/
cp ../environment/install.sh Halite-C#-Starter-Package/

# add halite.exe to all zip archives
mkdir Halite-Python-Starter-Package/bin
mkdir Halite-Java-Starter-Package/bin
mkdir Halite-C++-Starter-Package/bin
mkdir Halite-Rust-Starter-Package/bin
mkdir Halite-Scala-Starter-Package/bin
mkdir Halite-C#-Starter-Package/bin
cp downloads/environment/halite.exe Halite-Python-Starter-Package/bin/
cp downloads/environment/halite.exe Halite-Java-Starter-Package/bin/
cp downloads/environment/halite.exe Halite-C++-Starter-Package/bin/
cp downloads/environment/halite.exe Halite-Rust-Starter-Package/bin/
cp downloads/environment/halite.exe Halite-Scala-Starter-Package/bin/
cp downloads/environment/halite.exe Halite-C#-Starter-Package/bin/


zip -r Halite-Python-Starter-Package.zip Halite-Python-Starter-Package/ 
zip -r Halite-C++-Starter-Package.zip Halite-C++-Starter-Package/
zip -r Halite-Java-Starter-Package.zip Halite-Java-Starter-Package/ 
zip -r Halite-Rust-Starter-Package.zip Halite-Rust-Starter-Package/ 
zip -r Halite-C#-Starter-Package.zip Halite-C#-Starter-Package/ 
zip -r Halite-Scala-Starter-Package.zip Halite-Scala-Starter-Package/ 

mkdir -p ../website/downloads/starterpackages
mv *.zip ../website/downloads/starterpackages

rm -r *-Starter-Package
