import sys
predict = []
answers = []

with open(sys.argv[1], 'r') as result_f, open(sys.argv[2], 'r') as answer_f:
    for line in result_f:
        predict.append(line.strip().split()[0])
    for line in answer_f:
        answers.append(line.strip().split()[0])

correct = 0.
wrong = 0.

for ind in range(len(predict)):
    if(predict[ind] == answers[ind]):
        correct += 1
    else:
        wrong += 1

print correct / ind
