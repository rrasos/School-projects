"""
COMP.CS.100 Ohjelmointi 1
Tekijä: Rasmus Sorila
Sähköposti: rasmus.sorila@tuni.fi
Opiskelija numero: 150267845
Simple YATZY, only numbers 1-6 plus bonus, players 1-4, game rules can be found
in "options" menu bar.
"""

from tkinter import *
from tkinter import messagebox
from tkinter.messagebox import askyesno
from random import randint


class starting_screen:
    def __init__(self):
        self.root = Tk()
        self.root.title("YATZY")
        self.root.option_add("*Font", "Impact 16")
        self.root.configure(bg="#2c2c2c")
        self.components()
        self.create_menu()
        self.root.mainloop()

    """
    making components on starting screen
    pressing button opens new window for the game
    """
    def components(self):
        txt = Label(self.root, text="Welcome to  simple YATZY\n "
                                    "please select how many players",
                    bg="#2c2c2c", fg="white")
        txt.pack(fill=BOTH)

        for i in range(1, 5):
            players_button = Button(self.root, text=f'Players {i}',
                                    command=lambda number=i:
                                    self.start_game(number), width=5,
                                    highlightbackground="#2c2c2c")
            players_button.pack()

    """
    creates new window for the game, and starts it
    :param players: how many players are playing the game,
    :return: None
    """
    def start_game(self, players):

        GameGUI(self.root, players)

# =============================================================================
# menubar
    """
    Creates a menu bar with options 'Rules' and 'Quit'.
    """
    def create_menu(self):
        menu_bar = Menu(self.root)
        options_menu = Menu(menu_bar, tearoff=0)

        options_menu.add_command(label="Rules", command=self.show_rules)
        options_menu.add_command(label="Quit", command=self.quit_program)

        menu_bar.add_cascade(label="Options", menu=options_menu)
        self.root.config(menu=menu_bar)
    """
    Opens a popup window to confirm quitting the game.
    Destroys the window if 'Yes' is selected.
    """
    def quit_program(self):
        answer = askyesno(title="confirmation",
                          message="You really want to quit????")

        if answer:
            self.root.destroy()
    """
    Displays the rules from the 'Yatzy rules' file in a new window.
    Shows an error message if the file cannot be opened.
    """
    def show_rules(self):
        try:
            file = open("Yatzy rules", mode="r")
            rules = file.read()

            rules_window = Toplevel(self.root)
            rules_window.title("RULES")
            rules_window.configure(bg="#2c2c2c")

            rules_label = Label(rules_window,
                                text=rules, bg="#2c2c2c", fg="white",
                                font="Impact 21")
            rules_label.pack()
        except OSError:
            error = "error while opening rules"
            messagebox.showinfo("Error", error)


# =============================================================================
# Game window initialize

class GameGUI:
    def __init__(self, root, players):

        self.root = root
        self.root.option_add("*Font", "Impact 26")

        self.gamew = Toplevel(self.root)
        self.gamew.title(f"YATZY - {players} PLAYERS")
        self.gamew.configure(bg="#2c2c2c")

        self.turn_label = None
        self.points_label = None
        self.throws_label = None
        self.info_label = None
        self.instructions_label = None
        self.pip = None

        self.emojis = ["⚀", "⚁", "⚂", "⚃", "⚄", "⚅"]
        self.players = players
        self.buttons = []
        self.dices = []
        self.turn = 1
        self.throws = 3
        self.points = 0
        self.how_many = ""
        self.cells = []
        self.scores = []
        # create all players score to 0
        for i in range(0, players):
            score = 0
            self.scores.append(score)

        self.create_score_board()
        self.create_turn_label()
        self.create_check_button_dices()
        self.create_roll_dices_button()
        self.create_points_label()
        self.create_throws_left_label()
        self.check_throws()
        self.close_scores()

# =============================================================================
# game window buttons and labels
    """
    creates scoreboard;
    titles (ones, twos, etc.)
    buttons matrix 7 x players, to input and showcase scores. closes when
    score is inputted
    players total scores on the button
    and players on top of each column (P1, P2, ...)
    """
    def create_score_board(self):
        categories = [
            'Ones ⚀', 'Twos ⚁', 'Threes ⚂', 'Fours ⚃',
            'Fives ⚄', 'Sixes ⚅',
            'Bonus ?', 'Total Score:'
        ]
        # titles loop
        row = 1
        for t in categories:
            title = Label(self.gamew, text=t, font=("Impact", 26), width=10,
                          anchor=W, bg="#2c2c2c", fg="white")
            title.grid(row=row, column=0)
            row += 1
        # matrix loop
        for i in range(1, 8):
            row_buttons = []
            for j in range(self.players):
                button = Button(self.gamew, text="",
                                command=lambda x=i, y=j:
                                self.update_score(x, y), width=2,
                                highlightbackground="#2c2c2c",
                                disabledforeground="black")
                button.grid(row=i, column=j + 1)
                row_buttons.append(button)
            self.cells.append(row_buttons)

        # total score loop
        for i in range(1):
            for j in range(self.players):
                total = Label(self.gamew, text=self.scores[j],
                              bg="#2c2c2c", fg="white")
                total.grid(row=8, column=j + 1)
                self.buttons.append(total)

        # players
        for i in range(1):
            for j in range(self.players):
                total = Label(self.gamew, text=f'P{j + 1}',
                              bg="#2c2c2c", fg="white")
                total.grid(row=0, column=j + 1)

    """
    creates label on to right, to tell whose turn it is
    """
    def create_turn_label(self):
        if self.players > 1:
            self.turn_label = Label(self.gamew,
                                    text=f"Player {self.turn}'s turn",
                                    bg="#2c2c2c", fg="#faa3c3")
            self.turn_label.grid(row=0, column=4, columnspan=8)
    """
    creates 5 dices that are check buttons, when pressed it locks the dice
    """
    def create_check_button_dices(self):

        for i in range(5):
            ivar = IntVar()
            dice = Checkbutton(self.gamew, text=self.emojis[randint(0, 5)],
                               variable=ivar, bg="#1b1b1b", fg="#faa3c3", bd=4,
                               indicatoron=False, font=("Impact", 60), width=2)
            dice.grid(row=2, column=7 + i, rowspan=2)
            self.dices.append([dice, ivar])

    """
    creates button "roll dices" to roll the dices
    """
    def create_roll_dices_button(self):

        Button(self.gamew, text="Roll Dices", command=self.roll_dices,
               highlightbackground="#2c2c2c", relief="raised",
               highlightcolor="#faa3c3").grid(
            row=4, column=7, columnspan=5)

    """
    label that tells how many throws are left
    """
    def create_throws_left_label(self):
        self.throws_label = Label(self.gamew,
                                  text=f'Throws left: {self.throws}',
                                  bg="#2c2c2c", fg="white")
        self.throws_label.grid(row=5, column=7, columnspan=5)

    """
    top of the dices is shown how many points is in selected dices
    """
    def create_points_label(self):
        self.points_label = Label(self.gamew, text=f'{self.points}',
                                  bg="#2c2c2c", fg="white")
        self.points_label.grid(row=1, column=7, columnspan=5)

    """
    creates label that tells hos many dices you got (selected ones)
    tells instructions what to do
    checks how many players there is and makes correct label
    """
    def create_info_label(self):
        if self.how_many == "":
            score = "none"
            self.info_label = Label(self.gamew, text=f'You got {score} \n'
                                                f'input 0 on the score board'
                                                f' to move on',
                                    bg="#2c2c2c", fg="white",
                                    font=("Impact", 24))
            self.info_label.grid(row=6, column=7, columnspan=5, rowspan=3)

        elif self.players > 1 and self.how_many != "":
            self.info_label = Label(self.gamew,
                                    text=f'You got {self.how_many} \n'
                                         f'please input score on the '
                                         f'scoreboard\n'
                                         f'to move on the next player',
                                    font=("Impact", 22), bg="#2c2c2c",
                                    fg="white")
            self.info_label.grid(row=6, column=7, columnspan=5, rowspan=3)

        else:
            self.info_label = Label(self.gamew,
                                    text=f'You got {self.how_many} \n'
                                         f'please input score on the '
                                         f'scoreboard\n'
                                         f'to move on to the '
                                         f'next round',
                                    font=("Impact", 22),
                                    bg="#2c2c2c", fg="white")
            self.info_label.grid(row=6, column=7, columnspan=5, rowspan=3)

    """
    creates label "press roll dice to start"
    """
    def create_instructions(self):

        self.instructions_label = Label(self.gamew,
                                        text=f"Press roll dice to start",
                                        bg="#2c2c2c", fg="#faa3c3")
        self.instructions_label.grid(row=6, column=7, columnspan=5, rowspan=3)

# =============================================================================
# Methods
    """
    counts player total score and updates it,
    checks if all scores all filled and tells the winner
    :param x: row
    :param y: column
    """
    def update_score(self, x, y):

        self.cells[x - 1][y].configure(text=self.points, state=DISABLED)
        self.scores[y] += self.points
        self.buttons[y].configure(text=self.scores[y])
        self.points = 0
        self.roll_dices()
        self.get_winner()

    """
    rolls dices, activates them, deletes info label
    updates throws
    """
    def roll_dices(self):
        self.calculate_points()
        self.activate_dices()
        self.del_info()
        self.update_throws()
        for i in range(5):
            if self.dices[i][1].get() == 0:
                self.dices[i][0]["text"] = self.emojis[randint(0, 5)]

    """
    updates throws, and use other methods that need to be done in these
    specific situations
    """
    def update_throws(self):
        if self.throws == 0:
            self.throws -= 1
            self.create_info_label()
            self.close_dices()
            self.open_scores()

        elif self.throws == -1:
            self.throws = 3
            self.create_throws_left_label()
            self.change_turn()
            self.update_dices()
            self.close_dices()
            self.create_instructions()
            self.points = 0
            self.update_points()
            self.close_scores()

        else:
            self.throws -= 1
            self.create_throws_left_label()

    """
    changes whose turn it is and updates turn label, resets what dices is
    selected
    """
    def change_turn(self):
        self.turn += 1
        if self.turn > self.players:
            self.turn = 1

        self.create_turn_label()
        self.how_many = ""
        self.pip = None

    """
    deletes all information labels (info and instructions)
    """
    def del_info(self):
        if self.info_label is not None:
            self.info_label.destroy()
        if self.instructions_label is not None:
            self.instructions_label.destroy()

    """
    makes new dices
    """
    def update_dices(self):
        self.dices = []
        self.create_check_button_dices()

    """
    closes dices
    """
    def close_dices(self):
        for i in range(5):
            self.dices[i][0].configure(state=DISABLED)

    """
    checks dices state and if its disabled then activates dices
    """
    def activate_dices(self):
        if self.dices[0][0].cget("state") == "disabled":
            for i in range(5):
                self.dices[i][0].configure(state=ACTIVE)

    def check_throws(self):
        if self.throws == 3:
            self.close_dices()

    """
    calculates points, saves how many same dices are selected
    updates points
    """
    def calculate_points(self):
        try:
            self.points = 0
            selected_dices = []

            for i in range(5):
                if self.dices[i][1].get() == 1:
                    selected_dices.append(self.dices[i][0]["text"])
            amount = 0
            dice = ""
            for i in selected_dices:
                am = selected_dices.count(i)
                if am > amount:
                    amount = am
                    dice = i
                    self.how_many = f'{amount}x{dice}'

            self.pip = self.emojis.index(dice)
            self.points = (self.emojis.index(dice) + 1) * amount

            self.update_points()

        except ValueError:
            pass

    def update_points(self):
        self.points_label.destroy()
        self.create_points_label()

    """
    Disables score input for all players.
    """
    def close_scores(self):

        for i in range(1, 8):
            for j in range(self.players):
                self.cells[i - 1][j].configure(state=DISABLED)

    """
    opens on turn players available score inputs
    """
    def open_scores(self):
        if self.pip is not None:
            for i in range(1, 8):
                for j in range(self.players):
                    if j == self.turn - 1:
                        if self.cells[i - 1][j]["text"] == "":
                            if self.cells[self.pip][j]["text"] == "":
                                self.cells[self.pip][j].configure(state=NORMAL)
                        if self.cells[6][j]["text"] == "":
                            self.cells[6][j].configure(state=NORMAL)
        else:
            for i in range(1, 8):
                for j in range(self.players):
                    if j == self.turn - 1:
                        if self.cells[i - 1][j]["text"] == "":
                            self.cells[i - 1][j].configure(state=NORMAL)

    """
    Checks if all players have filled in all scores.
    Displays the winner if all scores are filled.
    """
    def get_winner(self):
        pogos = 0
        for i in range(1, 8):
            for j in range(self.players):
                if self.cells[i - 1][j]["text"] != "":
                    pogos += 1
                    if pogos == (self.players * 7):
                        win_score = 0
                        winner = 0
                        self.clear_window()
                        for s in range(self.players):
                            if self.scores[s] > win_score:
                                win_score = self.scores[s]
                                winner = s + 1
                                Label(self.gamew,
                                      text=f"Player {winner} Wins!!!",
                                      font="Impact 70").pack()

    """
    destroys all components on window
    """
    def clear_window(self):

        for widget in self.gamew.winfo_children():
            widget.destroy()


def main():
    starting_screen()


if __name__ == '__main__':
    main()
