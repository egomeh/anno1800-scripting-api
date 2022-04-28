using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;

namespace ProductionViewer
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            ((App)Application.Current).GameStateUpdate += new App.GameStateUpdateHandler(OnGameStateUpdate);
        }

        public void OnGameStateUpdate(Object sender, GameState gameState)
        {
            if (!IslandSelection.Dispatcher.CheckAccess())
            {
                IslandSelection.Dispatcher.Invoke(OnGameStateUpdate, sender, gameState);
                return;
            }

            foreach (string name in gameState.islandNames)
            {
                if (IslandSelection.Items.Contains(name))
                    continue;

                IslandSelection.Items.Add(name);
            }

            foreach (string name in IslandSelection.Items)
            {
                if (!gameState.islandNames.Contains(name))
                    IslandSelection.Items.Remove(name);
            }

            foreach (string name in gameState.resourceNames)
            {
                if (ResourceSelection.Items.Contains(name))
                    continue;

                ResourceSelection.Items.Add(name);
            }

            foreach (string name in ResourceSelection.Items)
            {
                if (!gameState.resourceNames.Contains(name))
                    ResourceSelection.Items.Remove(name);
            }

            string resourceKey = String.Format("{0}.{1}", IslandSelection.Text, ResourceSelection.Text);
            if (gameState.resources.ContainsKey(resourceKey))
            {
                ResourceInfo info = gameState.resources[resourceKey];
                StorageText.Text = String.Format("{0} / {1}", info.count, info.capacity);
                StorageBar.Value = ((float)info.count / (float)info.capacity) * 100.0f;

                ResidentConsumeValue.Text = String.Format("{0:0.00}", info.residential);
                IndustryConsumeValue.Text = String.Format("{0:0.00}", info.industry);

                float balance = info.industry - info.residential;
                
                if (balance == 0.0f)
                {
                    PredictionText.Text = "No change";
                }
                else if (balance < 0.0f)
                {
                    float minutes_to_expire = info.count / -balance;
                    PredictionText.Text = String.Format("Expires in {0:0.00} minutes", minutes_to_expire);
                }
                else
                {
                    float minutes_to_expire = (info.capacity - info.count) / balance;
                    PredictionText.Text = String.Format("Full in {0:0.00} minutes", minutes_to_expire);
                }
            }
        }
    }
}
