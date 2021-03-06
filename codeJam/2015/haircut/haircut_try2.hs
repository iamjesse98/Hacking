-- -------------------------------------------------------------------------------------
--         Author: Sourabh S Joshi (cbrghostrider); Copyright - All rights reserved.
--                       For email, run on linux (perl v5.8.5):
--   perl -e 'print pack "H*","736f75726162682e732e6a6f73686940676d61696c2e636f6d0a"'
-- -------------------------------------------------------------------------------------
import System.Environment (getArgs)
import Data.Ord
import Data.List
import Data.Maybe 

type Problem = (Int, [Int])
type Result  = Int 

main :: IO ()
main = do
  [file]  <- getArgs
  ip <- readFile file
  writeFile ((takeWhile (/= '.') file) ++ ".out" ) (processInput ip)

writeOutput :: [(Int, Result)] -> [String]
writeOutput = map (\(i, r) -> ("Case #" ++ (show i) ++ ": " ++ (writeResult r)))

processInput :: String -> String
processInput = unlines . writeOutput . zip [1..] . map solveProblem . parseProblem . tail . lines

writeResult :: Result -> String
writeResult = show

parseProblem :: [String] -> [Problem]
parseProblem [] = []
parseProblem (ns:ms:ss)  = (n, (map read . words $ ms)) : parseProblem ss
  where [b, n] = map read . words $ ns

-- returns chosen barber index (0 indexed), and all new times 
useFirstFreeBarber :: [(Int, Int)] -> (Int, [Int])
useFirstFreeBarber crs =
  let crsInit  = takeWhile ((/=0). fst) crs
      crs'     = dropWhile ((/=0) . fst) crs
      myBarber@(_, myr) = head crs'
      crsLater = tail crs'
  in  if null crs' -- no barber free
         then error "Something went wrong"
         else (length crsInit , (map fst crsInit) ++ [myr] ++ (map fst crsLater) )

-- given a list of barbers (time to completion)
-- makes one barber free, and decrements everyone elses time
makeBarberFree :: [Int] -> [Int]
makeBarberFree cs = 
  let alreadyFree = not . null . filter ((==0) ) $ cs
      ctime = minimum cs
  in  if alreadyFree then cs else map (subtract ctime) cs

-- given barbers (current times to completion, and reset times)
-- finishes one barber (if needed), and decrements others counts (if needed)
-- and returns (the barber that finished, new times to completion for all)
simulateHairCut :: [(Int, Int)] -> (Int, [Int])
simulateHairCut crs = 
  let newcs = makeBarberFree (map fst crs)
  in  useFirstFreeBarber (zip newcs (map snd crs))

-- finds the LCM of ALL the barbers RESET TIMES
-- all barbers serve ALL their customers repeatedly until the LCM time is reached
-- then returns total number of customers served, in this LCM time
lcmCustsServed :: [Int] -> Int
lcmCustsServed (r:rs) = 
  let lcmtime     = foldl' (\num lcmMid -> if lcmMid `mod` num == 0 then lcmMid else lcm lcmMid num) (last (r:rs)) $ (r:rs)
      custsServed = map (\time -> lcmtime `div` time) (r:rs)
  in  sum custsServed

solveProblem :: Problem -> Result
solveProblem pb@(n, ms) =
  let lcmCusts = lcmCustsServed ms
      leftovers= n `mod` lcmCusts
      minTime  = snd . minimumBy (comparing snd) $ zip [1..] ms
      allMinIndices = map fst . filter ((==minTime) . snd) $ zip [1..] ms
      initProb = zip (repeat 0) ms  -- barbers (times-to-completion, reset-times) 
      result = foldl' (\(_, pb) _ -> let (barb, newcs) = simulateHairCut pb in (barb, zip newcs ms)) (0, initProb)$ [1..leftovers]
  in  if leftovers == 0 then last allMinIndices else 1 + (fst result)


