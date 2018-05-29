'''
struct VertexAttribute
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
'''

def getIndex(str):
        items=str.split("/")
        while len(items)<3:
                items.append("")
        items=[-1 if item=="" else int(item)-1 for item in items]
        return items

def getData(data,index):
        if index==-1:
                return None
        return data[index]

def loadObjFile(fileName):
        try:
                print(fileName)
                lines=open(fileName).readlines()
                
                positions=[]
                normals=[]
                texCoords=[]

                vertexAttributes=[]

                for l in lines:
                        if l[:2:]=='vn':        #normal
                                normals.append([float(item) for item in  l.split()[1::]])
                        if l[:2:]=='vt':        #texture coord
                                texCoords.append([float(item) for item in  l.split()[1::]])
                        if l[0]=='v':        #position
                                positions.append([float(item) for item in  l.split()[1::]])
                        if l[0]=='f':        #face
                                indexStrItems=l.split()[1::]
                                for i in range(2,len(indexStrItems)):
                                        pIndex,tIndex,nIndex=getIndex(indexStrItems[0])
                                        vertexAttributes.append([
                                                getData(positions,pIndex),
                                                getData(normals,nIndex),
                                                getData(texCoords,tIndex),
                                                ])
                                        
                                        pIndex,tIndex,nIndex=getIndex(indexStrItems[i-1])
                                        vertexAttributes.append([
                                                getData(positions,pIndex),
                                                getData(normals,nIndex),
                                                getData(texCoords,tIndex),
                                                ])
                                        
                                        pIndex,tIndex,nIndex=getIndex(indexStrItems[i])
                                        vertexAttributes.append([
                                                getData(positions,pIndex),
                                                getData(normals,nIndex),
                                                getData(texCoords,tIndex),
                                                ])
                return vertexAttributes
        except Exception as e:
                print(e)

if __name__=='__main__':
        #model=loadObjFile(input("obj file name:"))
        model=loadObjFile("D:/Desktop/粒子系统/assignment3_source code/model_viewer/3d_models/bunny.obj")
        print(len(model))
        '''
        for m in model:
                print(m)
        '''
