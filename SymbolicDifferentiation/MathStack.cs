﻿using System.Collections.Generic;
using System.Linq;

namespace SymbolicDifferentiation
{
    internal class MathStack
    {
        public static readonly string[] PFunctions =
        {
            "(",
            "sin(",
            "cos(",
            "tan(",
            "sec(",
            "cosec(",
            "cot(",
            "sinh(",
            "cosh(",
            "tanh(",
            "sech(",
            "cosech(",
            "coth(",
            "asin(",
            "acos(",
            "atan(",
            "asec(",
            "acosec(",
            "acot(",
            "asinh(",
            "acosh(",
            "atanh(",
            "asech(",
            "acosech(",
            "acoth(",
            "sqrt(",
            "log10(",
            "log(",
            "ln(",
            "sign(",
            "abs("
        };

       

        public static int GetFunction(string pOpen, ref int nSign)
        {
            var p = 0;
            if (pOpen[p] == '-')
            {
                nSign = -1;
                ++p;
            }
            else if (pOpen[p] == '+')
            {
                nSign = 1;
                ++p;
            }
            for (var nIndex = 0; nIndex < PFunctions.Length; nIndex++)
            {
                if (pOpen.StartsWith(PFunctions[nIndex]))
                {
                    if (pOpen.GetClose(PFunctions[nIndex].Length - 1) == -1)
                    {
                        return -1;
                    }
                    return nIndex;
                }
            }
            return -1;
        }

        public static int GetOperator(string lpcs, string[] lpcsOperators)
        {
            if (lpcs.IsNumeric()) return -1;
            for (var nIndex = 0; nIndex < lpcsOperators.Length; nIndex++)
            {
                var nOpen = 0;
                // scan the expression from its end
                var p = lpcs.Length - 1;
                // loop tell reach expression start
                while (p >= 0)
                {
                    // check for close
                    if (lpcs[p] == ')')
                        nOpen++;
                    // check for open
                    else if (lpcs[p] == '(')
                        nOpen--;
                    // check for operator
                    else if (nOpen == 0 && lpcsOperators[nIndex].Contains(lpcs[p].ToString()))
                        // check if the operator in not a sign mark
                        if ((lpcs[p] != '-' && lpcs[p] != '+') ||
                            (p != 0 && IsRightSign(lpcs[p - 1], lpcsOperators, nIndex + 1)))
                            // return operator index
                            return p;
                    p--;
                }
            }
            // operator not found
            return -1;
        }


        private static bool IsRightSign(char c, string[] lpcsOperators, int nIndex)
        {
            if (lpcsOperators.Length == 1)
            {
                return !lpcsOperators.Contains(c.ToString());
            }
            for (; nIndex < lpcsOperators[nIndex].Length; nIndex++)
            {
                if (lpcsOperators.Contains(c.ToString()))
                    return false;
            }
            return true;
        }

        public static int FillStack(string strInput, List<ExpressionItem> vStack)
        {
            // operators array from high to low priority
            var lpcsOperators = ExpressionItem.Operators; //new [] { "+-", "*/", "^%"};

            // insert first input into the stack
            vStack.Add(new ExpressionItem(strInput));
            // loop in Expression stack to check if any Expression can be divided to two queries
            for (var nIndex = 0; nIndex < vStack.Count; nIndex++)
                // check if Expression item is operator
                if (vStack[nIndex].MCOperator == null)
                {
                    // copy Expression string
                    var str = vStack[nIndex].MStrInput;
                    // parse expression to find operators
                    var nOpIndex = GetOperator(str, lpcsOperators);
                    var sign = vStack[nIndex].MNSign;
                    if (nOpIndex != -1)
                    {
                        // split the Expression into two queries at the operator index
                        vStack[nIndex].MCOperator = str[nOpIndex];
                        // add the left operand of the operator as a new expression
                        var operand = str.Substring(0, nOpIndex);
                        //if (!operand.Contains(diffVar)) operand = "0";
                        vStack.Insert(nIndex + 1, new ExpressionItem(operand));
                        // add the right operand of the operator as a new expression
                        operand = str.Substring(nOpIndex + 1);
                        //if (!operand.Contains(diffVar)) operand = "0";
                        vStack.Insert(nIndex + 2, new ExpressionItem(operand));
                    }
                    else // check if Expression string starts with function or parenthesis
                        if ((vStack[nIndex].MNFunction = GetFunction(str, ref sign)) == 0 && sign == 0)
                        {
                            vStack[nIndex].MNSign = sign;
                            // remove parentheses and re-scan the Expression
                            vStack[nIndex--].MStrInput = str.Substring(1, str.Length - 2);
                        }
                }

            return 0;
        }
    }
}