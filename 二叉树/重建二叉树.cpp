// 根据前序和中序遍历，重建二叉树
/*
中序遍历：4,7,2,1,5,3,8,6
前序遍历：1,2,4,7,3,5,6,8
重建后二叉树：
        1
       / \
      2   3
     /   / \
    4   5   6
     \     /
      7   8
*/
#include<iostream>
using namespace std;

struct BinaryTreeNode{
    BinaryTreeNode(){m_pLeft=m_pRight=NULL;};
    int             value;
    BinaryTreeNode  *m_pLeft;
    BinaryTreeNode  *m_pRight;
    
};
BinaryTreeNode* constructCore
            (int *preStart,int *preEnd,
             int *midStart,int *midEnd);
BinaryTreeNode* construct(int *pre, int *mid, int length){
    if(pre==NULL||mid==NULL||length<=0)
        return NULL;
    return constructCore(pre, pre+length-1, mid, mid+length-1);
}

BinaryTreeNode* constructCore
            (int *preStart,int *preEnd,
             int *midStart,int *midEnd){
    // 前序遍历的第一个值为root
    BinaryTreeNode *root = new BinaryTreeNode();
    root->value = *preStart;
    
    // 出口
    if(preStart==preEnd){
        // 只剩1个单节点
        if(midStart==midEnd && *midStart==*preStart)
            return root;
        else
            throw "Invalid input";
    }
    // 在中序遍历中找到根节点的位置
    int *rootNode = midStart;
    while(rootNode!=midEnd && *rootNode!=*preStart)
        rootNode++;
    if(rootNode==midEnd && *rootNode!=*preStart)
        throw "Invalid input";
    
    // 左子树位置
    int Leftlength = rootNode - midStart;
    int *leftpreEnd = preStart+Leftlength;
    if(Leftlength>0){
        // 构建左子树
        root->m_pLeft = constructCore(preStart+1, leftpreEnd, midStart, rootNode-1);
    }
    
    //构建右子树
    if(preEnd-preStart>Leftlength){
        root->m_pRight = constructCore(leftpreEnd+1, preEnd, rootNode+1, midEnd);
    }
    return root;
                  
}

void backThrough(BinaryTreeNode* root){
    if(root==NULL) return;
    backThrough(root->m_pLeft);
    backThrough(root->m_pRight);
    cout<<root->value<<" ";
}
int main(){
    int pre[8]={1,2,4,7,3,5,6,8};
    int mid[8]={4,7,2,1,5,3,8,6};
    BinaryTreeNode *root=construct(pre, mid,8);
    backThrough(root);
    return 0;
}