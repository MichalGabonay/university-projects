--
-- FLP project 1: bkg-2-cnf
-- Michal Gabonay (xgabon00)
-- 2018
-- VUT FIT
-- 

module Bkg2cnfData where

import Data.List

---- Rule Data type
data Rule = Rule{ 
    left :: String,
    right :: [String]
} deriving (Eq)

instance Show Rule where
    show (Rule l r) = l ++ "->" ++ intercalate "" r

---- BKG Data type
data BKG = BKG {
    nonterms :: [String],
    terms :: [String],
    startNonterm :: String,
    rules :: [Rule]
} deriving (Eq) 

instance Show BKG where
    show (BKG n t s r) = 
        intercalate "," n ++ "\n" 
        ++ intercalate "," t ++ "\n" 
        ++ s ++ "\n" 
        ++ intercalate "\n" (map show r)