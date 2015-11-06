
predict = []
answers = []

with open('result1.txt', 'r') as result_f, open('../testing_answer.txt', 'r') as answer_f:
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
