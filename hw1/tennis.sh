#!/bin/bash

winner=0
players=( 50 50)
guesses=( 0 0 )

ballState=0
ballField=0
turnState=1
isGameOn=true

isInPlace=false

for arg in "$@"
do
    case $arg in
        --in-place|-p)
        isInPlace=true
        shift
        ;;
        -l|--log|--print-all)
        isInPlace=false
        shift
        ;;
    esac
done


printBoard () {
    if [[ $isInPlace == true ]]; then clear; fi
    echo " Player 1: ${players[0]}   🎾    Player 2: ${players[1]}"
    echo " --------------------------------- "
    echo " |       |       #       |       | "
    echo " |       |       #       |       | "
    case $ballState in
        -3)
            echo "O|       |       #       |       | "
            ;;
        -2) 
            echo " |   O   |       #       |       | "
            ;;
        -1)
            echo " |       |   O   #       |       | "
            ;;
        0)
            echo " |       |       O       |       | "
            ;;
        1)
            echo " |       |       #   O   |       | "
            ;;
        2)  
            echo " |       |       #       |   O   | "
            ;;
        3)
            echo " |       |       #       |       |O"
            ;;  
        *)
            echo " |       |       #       |       | "
            ;;
    esac
    echo " |       |       #       |       | "
    echo " |       |       #       |       | "
    echo " --------------------------------- "

}

setTurn () {
    if [[ $turnState == 1 ]]
    then
        turnState=2
    else
        turnState=1
    fi
}

printState () {
    echo -e "       Player 1 played: ${guesses[0]}\n       Player 2 played: ${guesses[1]}\n\n"
}

pickANumber () {
    player=$turnState-1;
    echo "PLAYER $turnState PICK A NUMBER: "
    read -s guess

    # validate pick
    if [[ ! $guess =~ ^-?[0-9]+$ ]] || (( $guess > ${players[($player)]} ))
    then
      echo "NOT A VALID MOVE !"
      pickANumber
    else # set guess and update score
      guesses[($player)]=$guess
      if (( $turnState == 2 )) 
      then
        calculateState
      fi
      players[($player)]=$((${players[($player)]}-$guess))
      setTurn
    fi
}

calculateState () {
    handLoser=0
    # if palyer 2 guessed smaller hand
    if (( ${guesses[1]} < ${guesses[0]} ))
    then
        handLoser=2
    else if (( ${guesses[1]} > ${guesses[0]} ))
         then
            handLoser=1
         fi 
    fi
    # checkWinner
    if (( $handLoser == 0 )); then return; fi

    if (( $handLoser == $ballField ))
    then
        ballState=$(( $ballState < 0 ? $ballState-1 : $ballState+1 ))
    else
        ballField=$handLoser
        ballState=$(( $handLoser == 1 ? -1 : 1 ))
    fi
}

checkWinner () {
    if (( $ballState == 3 || $ballState == -3 ))
    then
        winner=$(( 3-$ballField )) 
        isGameOn=false  
    else 
        if (( ${players[0]} == 0 ))
        then
            isGameOn=false  
            if (( ${players[1]} > 0 )); then 
                winner=2
            else 
                winner=$(( $ballField == 0 ? 0 : 3-$ballField ))
            fi
        else 
            if (( ${players[1]} == 0 ))
            then 
                winner=1
                isGameOn=false
            fi
        fi
    fi
}

printWinner () {
if  [ $isGameOn == false ]
    then
        case $winner in
            0)
            echo "IT'S A DRAW !"
            ;;
            1) 
            echo "PLAYER 1 WINS !"
            ;;
            2)
            echo "PLAYER 2 WINS !"
            ;;
            *)
            echo "$winner"
            ;;
        esac
    fi    
}

printBoard
while $isGameOn
do
  pickANumber
  pickANumber
  printBoard
  printState
  checkWinner
done
printWinner