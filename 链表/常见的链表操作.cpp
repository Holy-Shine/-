/*
1. 尾插操作addToTail
2. 删除特定节点RemoveNode
*/


#include <iostream>
using namespace std;

struct ListNode{
    int  value;
    ListNode *next;
};

/*****************************************************
 *func:添加结点到尾部
 *des:注意第一个参数必须是二重指针，因为可能是空链表插入
 *caution: 1.空链表输入
 ***************************************************/
void addToTail(ListNode **phead, int pval){
    ListNode *pnew = new ListNode;
    pnew->value = pval;
    pnew->next = NULL;
    // 空链表插入
    if(*phead==NULL){
        *phead = pnew;
    }
    else{
        ListNode *p=*phead;
        while(p->next!=NULL) p=p->next;
        p->next = pnew;
    }
}

/*****************************************************
 *func:删除链表某个元素
 *caution: 头结点删除
 ***************************************************/
void RemoveNode(ListNode **phead, int pval){
    if(phead==NULL||*phead==NULL)
        return;
    ListNode *nodeToBeDeleted = NULL;
    // 如果删除的是头结点
    if((*phead)->value==pval){
        nodeToBeDeleted = *phead;
        *phead = (*phead)->next;
    }
    else{
        ListNode *p = *phead;
        while(p->next!=NULL&&p->next->value!=pval)
            p = p->next;
        // 第一个条件：p指向最后一个节点代表没找到，不做任何操作
        if(p->next!=NULL && p->next->value==pval){
            nodeToBeDeleted = p->next;
            p->next=p->next->next;
        }
    }
    // 若找到
    if(nodeToBeDeleted!=NULL){
        delete nodeToBeDeleted;
        nodeToBeDeleted=NULL;
    }
}
/*****************************************************
 *func:打印链表
 ***************************************************/
void printList(ListNode *phead){
    while(phead){
        cout<<phead->value<<' ';
        phead=phead->next;
    }
    cout<<endl;
}

int main(){
    ListNode *phead = NULL;
    addToTail(&phead,1);
    addToTail(&phead,2);
    addToTail(&phead,3);
    printList(phead);
    RemoveNode(&phead,2);
    printList(phead);
    return 0;
}