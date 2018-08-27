--
-- FLP project 1: bkg-2-cnf
-- Michal Gabonay (xgabon00)
-- 2018
-- VUT FIT
-- 

module Main (main) where

-- Library imports
import System.Environment
import Data.List.Split
import Data.List
import System.IO
import Control.Monad

import Bkg2cnfData
import Bkg2cnfFuncs

-- MAIN ------------------------------------------------------------------
main :: IO ()
main = do 
    args <- getArgs

    let (setting, inFile) = procArgs args

    content <- getFileContent inFile

    let bkg = parseGrammare (lineUp content)

    unless (isValidBKG bkg) $ error "Wrong input grammare."

    case setting of 'i' -> print bkg
                    '1' -> print (remSimpleRulBKG bkg)
                    '2' -> print (transformToCnf $ remSimpleRulBKG bkg)
                    _ -> error "unknown argument"

    return()
---------------------------------------------------------------------------

-- Processing of arguments from console
procArgs :: [String] -> (Char,String)
procArgs [] = error "expects 1 or 2 arguments"
procArgs [x] = procArgs [x, ""]
procArgs [x,y]
    | x=="-i" = ('i', y)
    | x=="-1" = ('1', y)
    | x=="-2" = ('2', y)
    | otherwise = error "unknown argument"
procArgs _ = error "expects max 2 arguments"
