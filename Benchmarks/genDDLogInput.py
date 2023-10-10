import sys,re

class gendyck:
    NODES = {}
    NODES_ind = {}
    EDGETYPE = {}
    EDGETYPE_ind = {}
    node_index = 0
    edgetype_index = 0
    INITGRAPH = list()
    SEQUENCE = list()
    initgraphpath = ""
    seqpath = ""

    def __init__(self, initgraphpath, seqpath):
        self.initgraphpath = initgraphpath
        self.seqpath = seqpath

    def processInitgraph(self):
        with open(self.initgraphpath, "r") as file:
            for line in file:
                line = line.strip()
                pattern = r'([A-Za-z0-9_]+)->([A-Za-z0-9_]+)\[label="([A-Za-z0-9_-]+)"\]'
                matches = re.search(pattern, line.strip())
                if not matches:
                    print("regex does not match")
                    exit(1)
                node_from = matches.group(1)
                node_to = matches.group(2)
                edgeinfo = matches.group(3)

                edgedets = edgeinfo.split("--")
                edge_type = edgedets[1]
                if edgedets[0] == "op":
                    temp = node_from
                    node_from = node_to
                    node_to = temp

                if node_from not in self.NODES:
                    self.NODES[node_from] = self.node_index
                    self.NODES_ind[self.node_index] = node_from
                    # print(f"insert EMPTY({node_from},{node_from});")
                    # print(f"insert EMPTY({self.node_index},{self.node_index});")
                    # print(f"insert NODE({self.node_index});")
                    print(f"insert NODE({node_from});")
                    self.node_index = self.node_index + 1

                if node_to not in self.NODES:
                    self.NODES[node_to] = self.node_index
                    self.NODES_ind[self.node_index] = node_to
                    # print(f"insert EMPTY({node_to},{node_to});")
                    # print(f"insert EMPTY({self.node_index},{self.node_index});")
                    # print(f"insert NODE({self.node_index});")
                    print(f"insert NODE({node_to});")
                    self.node_index = self.node_index + 1

                if edge_type not in self.EDGETYPE:
                    self.EDGETYPE[edge_type] = self.edgetype_index
                    self.EDGETYPE_ind[self.edgetype_index] = edge_type
                    self.edgetype_index = self.edgetype_index + 1

                edge = [self.NODES[node_from], self.NODES[node_to], self.EDGETYPE[edge_type]]
                self.INITGRAPH.append(edge)

    def processSequence(self):
        
        with open(self.seqpath, "r") as file:
            for line in file:
                line = line.strip()
                content = line.split()
                
                oper = content[0]
                node_from = content[1]
                node_to = content[2]
                edgeinfo = content[3]

                edgedets = edgeinfo.split("--")
                edge_type = edgedets[1]
                if edgedets[0] == "op":
                    temp = node_from
                    node_from = node_to
                    node_to = temp

                if node_from not in self.NODES:
                    self.NODES[node_from] = self.node_index
                    self.NODES_ind[self.node_index] = node_from
                    # print(f"insert EMPTY({self.NODES_ind[self.node_index]},{self.NODES_ind[self.node_index]});")
                    # print(f"insert EMPTY({self.node_index},{self.node_index});")
                    # print(f"insert NODE({self.node_index});")
                    print(f"insert NODE({node_from});")
                    self.node_index = self.node_index + 1
                if node_to not in self.NODES:
                    self.NODES[node_to] = self.node_index
                    self.NODES_ind[self.node_index] = node_to
                    # print(f"insert EMPTY({self.NODES_ind[self.node_index]},{self.NODES_ind[self.node_index]});")
                    # print(f"insert EMPTY({self.node_index},{self.node_index});")
                    # print(f"insert NODE({self.node_index});")
                    print(f"insert NODE({node_to});")
                    self.node_index = self.node_index + 1
                if edge_type not in self.EDGETYPE:
                    self.EDGETYPE[edge_type] = self.edgetype_index
                    self.EDGETYPE_ind[self.edgetype_index] = edge_type
                    self.edgetype_index = self.edgetype_index + 1

                edge = [oper, self.NODES[node_from], self.NODES[node_to], self.EDGETYPE[edge_type]]
                self.SEQUENCE.append(edge)

    def parseInitgraph(self):
        # print(self.INITGRAPH)
        for edge in self.INITGRAPH:
            fromNodeID = edge[0]
            toNodeID = edge[1]
            edgeTypeID = edge[2]
            # print(f"insert CP({edgeTypeID},{fromNodeID},{toNodeID});")
            print(f"insert CP({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[fromNodeID]},{self.NODES_ind[toNodeID]});")
            # print(f"insert RA({edgeTypeID},{fromNodeID},{toNodeID});")
            # print(f"insert RB({edgeTypeID},{toNodeID},{fromNodeID});")
            # print(f"insert RA({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[fromNodeID]},{self.NODES_ind[toNodeID]});")
            # print(f"insert RB({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[toNodeID]},{self.NODES_ind[fromNodeID]});")

    def parseSeq(self):
        # print(self.SEQUENCE)
        for edge in self.SEQUENCE:
            fromNodeID = edge[1]
            toNodeID = edge[2]
            edgeTypeID = edge[3]
            if edge[0] == "A":
                # print(f"insert CP({edgeTypeID},{fromNodeID},{toNodeID});")
                print(f"insert CP({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[fromNodeID]},{self.NODES_ind[toNodeID]});")
                # print(f"insert RA({edgeTypeID},{fromNodeID},{toNodeID});")
                # print(f"insert RB({edgeTypeID},{toNodeID},{fromNodeID});")
                # print(f"insert RA({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[fromNodeID]},{self.NODES_ind[toNodeID]});")
                # print(f"insert RB({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[toNodeID]},{self.NODES_ind[fromNodeID]});")
            elif edge[0] == 'D':
                # print(f"delete CP({edgeTypeID},{fromNodeID},{toNodeID});")
                print(f"delete CP({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[fromNodeID]},{self.NODES_ind[toNodeID]});")
                # print(f"delete RA({edgeTypeID},{fromNodeID},{toNodeID});")
                # print(f"delete RB({edgeTypeID},{toNodeID},{fromNodeID});")
                # print(f"delete RA({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[fromNodeID]},{self.NODES_ind[toNodeID]});")
                # print(f"delete RB({self.EDGETYPE_ind[edgeTypeID]},{self.NODES_ind[toNodeID]},{self.NODES_ind[fromNodeID]});")
            else:
                pass
            print("commit;")
            print("start;")
        print("commit;")
        print("timestamp;")

    def generate(self):
        print("start;")
        self.processInitgraph()
        self.processSequence()
        self.parseInitgraph()
        print("commit;")
        print("start;")
        print("timestamp;")
        self.parseSeq()

if len(sys.argv) != 3:
    print("Usage: python3 genDDLogInput.py initgraph.dot sequence.seq\nConverts given initialGraph and sequenceFile to DDLog input")
    exit(1)

obj = gendyck(sys.argv[1],sys.argv[2])
obj.generate()
