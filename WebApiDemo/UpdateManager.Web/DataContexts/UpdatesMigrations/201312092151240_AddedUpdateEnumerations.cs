namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedUpdateEnumerations : DbMigration
    {
        public override void Up()
        {
            AddColumn("UpdateManager.Updates", "Type", c => c.Int(nullable: false));
            AddColumn("UpdateManager.Updates", "Status", c => c.Int(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("UpdateManager.Updates", "Status");
            DropColumn("UpdateManager.Updates", "Type");
        }
    }
}
