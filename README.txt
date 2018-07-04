For this exercise sheet we did not do the last "low priority" optimization exercise.

The integration test runs smoothly except for 1 example.

When running integrations tests even_pi.mC fails with the last digit being one off
(expected: 3.137594, Got: 3.137593)
Unfortunately we do not know why.

For testing the assembler code we originally were using a self written testing framework in the mC
language (/doc/test_mCc/test1.mC).

We then further on added a few google tests, which only compare outputs for some simple code snippets.
