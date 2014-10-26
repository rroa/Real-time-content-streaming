namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class DefaultSchema : DbMigration
    {
        public override void Up()
        {
            MoveTable(name: "dbo.Updates", newSchema: "UpdateManager");
        }
        
        public override void Down()
        {
            MoveTable(name: "UpdateManager.Updates", newSchema: "dbo");
        }
    }
}
