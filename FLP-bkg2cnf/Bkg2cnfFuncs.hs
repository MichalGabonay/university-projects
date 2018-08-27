--
-- FLP project 1: bkg-2-cnf
-- Michal Gabonay (xgabon00)
-- 2018
-- VUT FIT
-- 

module Bkg2cnfFuncs
    (parseGrammare, getRules, parseRules, isValidBKG, isValidTerms, isValidNonTerms,
    existingNonTerm, isValidRules, isValidRule, termsOrNonterms,getNewNonterms,
    createHelpSet, makeNonSimpleRules, removeSimpleRules, remSimpleRulBKG, checkIfRuleSimple,
    transformToCnf, lineUp, getFileContent,isValidCnfRule,rulesToCNF,remakeRule,
    createComplexNonterm,transformTerms,termToNonterm)
  where

import System.Environment
import Data.List.Split
import Data.List
import System.IO

import Bkg2cnfData

------------------------------------
-- PARSING AND VALIDATIONG OF INPUT
------------------------------------

-- put input string into lines
lineUp :: String -> [String]
lineUp = Prelude.lines

-- Reads input into a string
getFileContent :: String -> IO String
getFileContent [] = getContents
getFileContent x = readFile x

-- Create from input string context free grammare
parseGrammare :: [String] -> BKG
parseGrammare x = 
    if length x < 4
        then error "Wrong input grammare1."
        else
            BKG {
                nonterms = splitOn "," (head x), 
                terms = splitOn "," ( x !! 1), 
                startNonterm = x !! 2, 
                rules = getRules x
            }

-- proccess all rules into right form for Rule
getRules :: [String] -> [Rule] 
-- getRules x = map (parseRules) (tail $ tail $ tail x)
getRules = map parseRules . drop 3 . filter (not . null)

-- create crom line string rule
parseRules :: String -> Rule
parseRules x = 
    if length ruleParts == 2
        then Rule {left = head ruleParts, right = filter (/="") (splitOn "" (head $ tail ruleParts))}
        else error "Wrong input grammare2."
    where ruleParts = splitOn "->" x

-- check if input grammare has correct syntax 
isValidBKG :: BKG -> Bool
isValidBKG (BKG n t s r) = 
    isValidNonTerms n && isValidTerms t && existingNonTerm s n && isValidRules r n t

-- check if all terms have correct format
isValidTerms :: [String] -> Bool
isValidTerms x = all (==1) (map length x) && all (`elem` ['a'..'z']) (map head x)

-- check if all nonterms have correct format
isValidNonTerms :: [String] -> Bool
isValidNonTerms x = all (==1) (map length x) && all (`elem` ['A'..'Z']) (map head x)

-- check if this nonterm is in grrammare 
existingNonTerm :: String -> [String] -> Bool
existingNonTerm s n = any (`elem` s) (map head n)   

-- check if all rules have correct format
isValidRules :: [Rule] -> [String] -> [String] -> Bool
isValidRules [] n t = False
isValidRules [r] n t = isValidRule r n t
isValidRules (r:rs) n t = isValidRule r n t && isValidRules rs n t

-- check if rule has correct format
isValidRule :: Rule -> [String] -> [String] -> Bool
isValidRule (Rule l r) n t = existingNonTerm l n && termsOrNonterms r n t

-- check if symbols are in grammare term or nonterm declaration
termsOrNonterms :: [String] -> [String] -> [String] -> Bool
termsOrNonterms [] n t = False
termsOrNonterms [c] n t = any (`elem` c) (map head (n++t))
termsOrNonterms (c:xs) n t = any (`elem` c) (map head (n++t)) && termsOrNonterms xs n t

------------------------------------------------------------------------------------------------
------------------------
-- REMOVE SIMPLE RULES
------------------------

-- create set for removing simple rules (TIN - Algorithm 4.5 - 1. step )
createHelpSet :: Int -> [String] -> [Rule] -> [Rule] -> [String]
createHelpSet k n [] ro = []
createHelpSet k n [Rule l r] ro
    | all (`elem` n) [l] && checkIfRuleSimple (Rule l r) = 
        if length (nub (r++n)) /= k
            then createHelpSet (length (nub (r++n))) (nub (r++n)) ro ro  
            else nub (r++n)
    | otherwise =
        if length (nub n) /= k
            then createHelpSet (length (nub n)) (nub n) ro ro  
            else nub n
createHelpSet k n (Rule l r : r2) ro
    | all (`elem` n) [l] && checkIfRuleSimple (Rule l r) = nub (r ++ createHelpSet k (nub (r++n)) r2 ro)
    | otherwise = createHelpSet k n r2 ro

-- create nonsimple rules (TIN - Algorithm 4.5 - 2. step (a))
makeNonSimpleRules :: [String] -> [String] -> [Rule] -> [Rule]
makeNonSimpleRules n hs [Rule l r]
    | all (`elem` hs) [l] && not (checkIfRuleSimple (Rule l r)) = [Rule (head n) r]
    | otherwise = []
makeNonSimpleRules n hs (Rule l r : rs)
    | all (`elem` hs) [l] && not (checkIfRuleSimple (Rule l r)) = 
        Rule (head n) r : makeNonSimpleRules n hs rs
    | otherwise = makeNonSimpleRules n hs rs

-- for each nonterm create nonsimple rules (TIN - Algorithm 4.5 - 2. step (b))
removeSimpleRules :: [String] -> [Rule] -> [Rule]
removeSimpleRules [n] r = makeNonSimpleRules [n] (createHelpSet 1 [n] r r) r
removeSimpleRules (n:ns) r = makeNonSimpleRules [n] (createHelpSet 1 [n] r r) r ++ removeSimpleRules ns r

-- remove simple rules from grammare
remSimpleRulBKG :: BKG -> BKG
remSimpleRulBKG (BKG n t s r) = BKG n t s (nub (removeSimpleRules n r))

-- check if rule is simple
checkIfRuleSimple :: Rule -> Bool
checkIfRuleSimple (Rule l [r])
    | head r `elem` ['A'..'Z'] = True
    | otherwise                  = False
checkIfRuleSimple (Rule l r) = False

------------------------------------------------------------------------------------------------
-----------------------------------------------------------------
-- recreate context free grammare into CNF (Chomsky normal form)
-----------------------------------------------------------------

-- check if rule have right form for CNF
isValidCnfRule :: [String] -> Bool
isValidCnfRule r
    | length r == 1 && all (==1) (map length r) && all (`elem` ['a'..'z']) (map head r) = True
    | length r == 2 && all (`elem` ['A'..'Z']) (map head r) = True
    | otherwise = False

-- put all rules into CNF form
rulesToCNF :: [Rule] -> [Rule]
rulesToCNF [Rule l r]
    | isValidCnfRule r  = [Rule l r]
    | otherwise         = remakeRule (Rule l r) []
rulesToCNF (Rule l r : rs)
    | isValidCnfRule r  = Rule l r : rulesToCNF rs
    | otherwise         = remakeRule (Rule l r) [] ++ rulesToCNF rs

-- create new rules from non CNF rule
remakeRule :: Rule -> [Rule] -> [Rule]
remakeRule (Rule l r) rule
    | length r == 2 = rule ++ [Rule l r]
    | otherwise = remakeRule newRuleIn (rule ++ [newRuleOut])
        where newRuleOut = Rule l (head r : [createComplexNonterm r])
              newRuleIn  = Rule (createComplexNonterm r) (tail r)

-- create complex nonterm for CNF form
createComplexNonterm :: [String] -> String
createComplexNonterm xs = concat $ ["<"] ++ tail xs ++ [">"]

-- put new created rules
transformTerms :: [Rule] -> [Rule]
transformTerms [Rule l [r1, r2]]  = termToNonterm l [r1, r2]
transformTerms [Rule l [r1]]     = [Rule l [r1]]
transformTerms (Rule l [r1, r2] : rs)  = termToNonterm l [r1, r2] ++ transformTerms rs
transformTerms (Rule l [r1] : rs)     = Rule l [r1] : transformTerms rs

-- create rules for terms in rules (a'->a)
termToNonterm :: String -> [String] -> [Rule]
termToNonterm l [r1, r2]
    | r1 /= r1New && r2 /= r2New = [Rule l [r1New,r2New], Rule r1New [r1], Rule r2New [r2]]
    | r1 /= r1New                = [Rule l [r1New,r2New], Rule r1New [r1]]
    | r2 /= r2New                = [Rule l [r1New,r2New], Rule r2New [r2]]
    | otherwise                  = [Rule l [r1,r2]]
    where r1New = if head r1 `elem` ['a'..'z'] then r1 ++ "\'" else r1
          r2New = if head r2 `elem` ['a'..'z'] then r2 ++ "\'" else r2

-- get new nonterm for grammare (<ABC>, a')
getNewNonterms :: [Rule] -> [String]
getNewNonterms [Rule l r] = [l]
getNewNonterms (Rule l r : rs) = l : getNewNonterms rs

-- transform grammare (already without simple rules) into CNF
transformToCnf :: BKG -> BKG
transformToCnf (BKG n t s r) = BKG newNonterms t s newRules
    where newRules = nub (transformTerms $ rulesToCNF r)
          newNonterms = nub (n ++ getNewNonterms newRules)

-----------------------------------------------------------------------------------------