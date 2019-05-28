
import STcpClient

'''
    輪到此程式移動棋子
    board : 棋盤狀態(list of list), board[i][j] = i row, j column 棋盤狀態(i, j 從 0 開始)
            0 = 空、1 = 黑、2 = 白
    is_black : True 表示本程式是黑子、False 表示為白子

    return step
    step : list of list, step = [(r1, c1), (r2, c2) ...]
            r1, c1 表示要移動的棋子座標 (row, column) (zero-base)
            ri, ci (i>1) 表示該棋子移動路徑
'''
def GetStep(board, is_black):
    # fill your program here
    pass

while(True):
    (stop_program, id_package, board, is_black) = STcpClient.GetBoard()
    if(stop_program):
        break
    
    listStep = GetStep(board, is_black)
    STcpClient.SendStep(id_package, listStep)
