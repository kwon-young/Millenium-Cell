class Cell
  attr_accessor :reaction

  def initialize(state=:healthy)
    @state = state
    affect_reaction
  end

  def affect_reaction
    reaction_name = @state.to_s.capitalize + "Reaction"
    @reaction = Reaction.const_get reaction_name
  end

  def react
    # we have to define the input
    reaction.perform input
  end
end

class CellTissue
  attr_accessor :cells

  DEFAULT_SIZE = 20

  def initialize(size=DEFAULT_SIZE)
    @cells  = Array.new(size, Array.new(size))
    @energy = Array.new(size, Array.new(size, 0))
  end
end

module Reaction
  module Base
    def perform(input)
      processus input
    end

    def processus(input)
      raise "The processus method have to be specialized."
    end
  end

  module HealthyReaction
    include Base

    def processus(input)
      if input[:oxygen] and input[:glucose]
        breathing
      else not input[:oxygen] and input[:glucose]
        fermentating
      end
    end

    def breathing
      36
    end

    def fermentating
      2
    end
  end

  module CancerousReaction
    include Base

    def processus(input)
      breathing_and_fermentating
    end

    def breathing_and_fermentating
      4
    end
  end
end
