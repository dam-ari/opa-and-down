# OpSystems

1. <details open><summary><h3>  Ex1: ⚒️ Gcc Find; 🎾 Tennis ; 💲Shell </h3></summary>
     <p>
     
    _____________________________________
     > <details><summary> <h3>⚒️ Gcc Find:</h3><br> </summary> 
     > a nice little script for compiling all the c files in the folder that contains a specific word. You can turn on th -r flag for recessiveness 
     ><br> <br> <b>⚙️How To⚙️</b><br> <br> 
     > <ul> <li> Clone the script to a folder of your choice<br> 
     >  <li> Run the gcc.sh file with the following params:<br>
      >  1. folder name to be compiled <br>
      >  2. a word. only files that contains the given word will be complied <b>::must!</b><br>
      >  3. optional: -r flag, if you want to compile the inner  files as well. <br> </ul>
   >
   >⚡<code> > ./gccfind.sh myFolder main -r </code> 
           </details>
    _____________________________________

    > <details open><summary> <h3>🎾 Tennis (paper game):</h3><br> </summary> 
    > a funny little [tennis paper game](https://en.wikipedia.org/wiki/Tennis_(paper_game)), terminal styled.
    > #### <b>⚙️How To⚙️</b>
    > There are two options for the game: in place game, and logged history game
    > You can choose what to use by simply run the game with flags:
     > * __for in place game:__  --in-place | -p
     > * __for logged game :__  --l | -log
     > * The default version is logged  <br>
    > 
    > ⚡<code> > ./tennis.sh -p </code> 
     _____________________________________   
     > <details><summary> <h3>💲 Shell:</h3><br> </summary> 
    > a shell program that can be loaded with your custom commands
    > <br> <br> <b>⚙️How To⚙️</b><br> <br> 
     > <ul> <li>compile myshell.c <code>gcc -o shell myshell.c </code> <br>
     > <li><strong> There are two options: </strong><br>
     > you can run the runnable script without any other params. <br>
     > The program will act as you expect from a normal shell <br>
     > <i>Or</i> you can send path to a folder, which contains custom scrits. The folder be then added temporarily to your env variables, <br>
     > so you can can any script that is defined there by simply typing it's name. <br> <br> </ul>
      > <b> for example:</b>
      >    if inside a folder named 'myCommands' you have a script named 'do-somthing.sh' <br>
      > you can run the shell with the path to 'myCommads', then you can just type 
     >  <code>do-something.sh </code>,<br>  no matter what pwd you are in.     <br>     
    > <br> 
    > ⚡<code> > ./myshell ~/myCommands </code>  </details>
     _____________________________________
          
     </p>
     </summary>
     </details>
_______________________________________________________     

2. <details open><summary><h3>  Ex2: 📂 File Comperator & Grader </h3></summary>
     <p> 
             
    _____________________________________
     > <details open><summary> <h3> 2.1: Files Comperator </h3><br> </summary>
     > a program that get two arguments of file pathes and determines the similarity:<br> 
     >    🔖 1: The files are identical <br> 
     >    🔖 2: The files are different <br> 
     >    🔖 3: The files are similar <br> 
     > <br> 
     > ⚡<code> > ./comp.out  /home/os20 12 /code/1.txt  /home/os20 12 /code/2.txt  </code>
     >     </summary>
     >    </details>
             
    _____________________________________
     > <details><summary> <h3> 2.2: Grader </h3><br> </summary>
     > A program that gets config path and grades the folders by that <br> 
     >    The config file contains 3 rows: <br>
     >    ✏️ 1: Path to parent folder which contains sub-folders for grade. Each folder is a user folder. <br> 
     >    ✏️ 2: Path to an input file for will-be executed program <br> 
     >    ✏️ 3: Path to correct output file <br> 
     >    The program iterates through the users' folders, compiles(if exists) the c file in that folder.
     >    Then runs the compiled program with the input file and comperes the results to the correct output from the config file.
     >    If anything in the way is getting wrong, the grade will be affected as listing here: <br>
     >          > > > > >       ✨ NO_C_FILE : 0 <br>
     >          > > > > >       ✨ COMPILATION_ERROR: 10 <br>
     >          > > > > >       ✨ TIMEOUT: 20 <br>
     >          > > > > >       ✨ WRONG: 50 <br>
     >          > > > > >       ✨ SIMILAR: 75 <br>
     >    The grade will be written into an excel file, named results.csv, which will be created inside the running folder.<br> 
     >    <br> 
     > ⚡<code> >  ./a.out /home/os20 12 /conf.txt </code>
     >     </summary>
     >    </details>
    _____________________________________
     </p>
     </summary>
     </details>
_______________________________________________________  

3. <details open><summary><h3>  Ex3: 🗞️ Publisher: content producer-distributer-coeditor  Threads Synchronization</h3></summary>
     <p> 
             
    _____________________________________
     > A program that gets a config file for products per procuder, and coediting abilities.
     > The program syncronizing the work of everyone by threads
     > <br> To create executable you can use the makefile 
     > ⚡<code> > ./ex3.out config.txt  </code>
    _____________________________________
     </p>
     </summary>
     </details>
_______________________________________________________            
