
struct ListNode{
  int value;
  ListNode *next;
};

// 用栈
void printReverseList(ListNode *pHead){
    std::stack<ListNode *> nodes;
    ListNode *pNode = *pHead;
    while(pNode!=NULL){
        nodes.push(pNode);
        pNode = pNode->next;
    }
    while(!nodes.empty()){
        pNode = nodes.top();
        printf("%d\t", pNode->value);
        nodes.pop();
    }
}

// 既然想到用栈，可以改成递归，如果对调用栈需求不大
void recurvePrintList(ListNode *pHead){
    if(pHead==NULL)
        return;
    recurvePrintList(pHead->next);
    cout<<pHead->value;
}